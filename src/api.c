#include "api.h"

int init_CAN(uint8_t nodeID){
    // Global interrupts
    sei();
    
    // Software reset; necessary for all CAN
    // stuff.
    CANGCON = _BV(SWRES);

    // CAN prescaler timing prescaler set to 0
    CANTCON = 0x00; 

    // Set BAUD rate
    CANBT1 = 0x01; // Set BAUD rate to 500kb
    CANBT2 = 0x04; // Re-synch handling
    CANBT3 = 0x13; // Phase edge error handling

    // enable interrupts: all, receive
    CANGIE = (_BV(ENIT) | _BV(ENRX));

    // compatibility with future chips
    CANIE1 = 0;

    // enable interrupts on all MObs
    CANIE2 = (_BV(IEMOB0) | _BV(IEMOB1) |
              _BV(IEMOB2) | _BV(IEMOB3) | 
              _BV(IEMOB4) | _BV(IEMOB5) );


    // All MObs come arbitrarily set up at first,
    // must reset all in order to make them useable
    int8_t mob;
    for( mob=0; mob<6; mob++ ){
        // Selects Message Object 0-5
        // This changes the MOb that is selected
        CANPAGE = ( mob << 4 );

        // Disable mob
        CANCDMOB = 0x00;
        // Clear mob status register;
        CANSTMOB = 0x00;
    }

    setup_mobs(nodeID);

    // Set up as Enabled mode
    //  instead of standby
    //  Necessary in order to get CAN
    //  communication
    CANGCON |= _BV( ENASTB );

    return(0);
}

void setup_mobs(uint8_t nodeID){
    // Set MOb1 for storing this nodes messages
    CANPAGE = _BV(MOBNB0);

    // MOb ID/IDmsk settings
    // set compatibility registers to 0, RTR/IDE-mask to 1
    CANIDM4 = (_BV(RTRMSK) | _BV(IDEMSK)); // write to 0x00?
    CANIDT4 = 0x00;

    CANIDM3 = 0x00;
    CANIDT3 = 0x00;

    // accept all message IDs (bits 0-5)
    // accept only this node's node ID (bits 6-10)
    CANIDM2 = 0x00;
    CANIDT2 = 0x00;

    CANIDM1 = 0x00; //This node will recieve all messages
    //CANIDM1 = 0xF8; // 0b11111000

    CANIDT1 = ((nodeID & 0x1F) << 3); // node ID

    // enable reception, DLC8
    CANCDMOB = _BV(CONMOB1) | (8 << DLC0);
}

void read_msg(void){
    CANPAGE &= ~(_BV(AINC) | _BV(INDX2) | _BV(INDX1) | _BV(INDX0)); // set data page 0
    uint8_t msgLength = (CANCDMOB & 0x0F); // last 4 bits are the DLC (0b1111)
    uint8_t receivedMsg[msgLength];

    // read the data into a local memory block
    int i;
    for (i = 0; i < msgLength; ++i) {
        //while data remains, read it
        receivedMsg[i] = CANMSG;
    }

    // take all of IDT1 and the first 3 bits of IDT2
    uint16_t idtag = (_BV(CANIDT1) << 3) | ((_BV(CANIDT2) & 0xE0) >> 5);
    uint8_t nodeID = (idtag & 0x07C0); // nodeID is bits 6-10 (0b11111000000)
    uint8_t msgID = (idtag & 0x003F); // msgID is bits 0-5 (0b111111)

    // externally-defined handler method
    handle_CAN_msg(nodeID,msgID,receivedMsg,msgLength);

    CANCDMOB |= _BV(CONMOB1); // set up MOb for reception again
}


// Sample call: sendCANmsg(NODE_watchdog,MSG_critical,data,dataLen);
int send_CAN_msg(uint8_t destID, uint8_t msgID, uint8_t msg[], uint8_t msgLength) {
    // use MOb 0 for sending and auto-increment bits in CAN page MOb register
    //CANPAGE = 0;
    //CANPAGE = _BV(MOBNB0);
    

    //Wait for MOb1 to be free
    // TODO: This is not good practice; take another look later
   while(CANEN2 & _BV(ENMOB0)); // Stuck in infinite loop?
    //if( CANEN2 & _BV(ENMOB0) ){
    //    return -2;
    //}
    //
    CANPAGE = 0x0; // Select MOb0

    CANEN2 |= (1 << ENMOB0); //Claim MOb1

    //Clear MOb status register
    CANSTMOB = 0x00;

    int i;
    for (i = 0; i < msgLength; ++i) {
        // while data remains, write it into the data page register
        CANMSG = msg[i];
    }

    // set compatibility registers, RTR bit, and reserved bit to 0
    CANIDT4 = 0;

    CANIDT3 = 0;

    // set ID tag registers
    uint16_t idtag = ((destID & 0x1F) << 6) | (msgID & 0x3F);
    CANIDT2 = ((idtag & 0x07) << 5); // bits 0-2 of idtag (0b0111)
    CANIDT1 = ((idtag & 0x7F8) >> 3); // bits 3-10 of idtag (0b11111111000)

    CANCDMOB = (_BV(CONMOB0) | msgLength); // set transmit bit and data length bits of MOb control register

    //TODO: use interrupts for this instead of while loop
    //wait for TXOK
    //while((CANSTMOB & (1 << TXOK)) != (1 << TXOK));// & timeout--);
    //uint8_t timeout = 0;
    while( !(CANSTMOB & ( 1 << TXOK )));
    //for( timeout = 0; timeout < 100; timeout++ ){
    //    _delay_ms(5);
    //}

    CANCDMOB &= ~(_BV(CONMOB0));
    
    
    //if( (CANSTMOB & (1 << TXOK)) != (1 << TXOK)){
    //    return -2;
    //}

    //Disable Transmission
    CANCDMOB = 0x00;
    //Clear TXOK flag (and all others)
    CANSTMOB = 0x00;

    return 0;
}


// handles the CAN interrupts depending on what kind of interrupt it is
ISR(CAN_INT_vect) {
    char cSREG = SREG; //store SREG

    uint8_t mobIndex = (CANHPMOB & 0xF0) >> 4; // check which MOb received the interrupt
    CANPAGE &= 0x0F; // clear out the top 4 bits (current MOb)
    CANPAGE |= mobIndex << 4; // set the current MOb

    if (CANSTMOB & _BV(RXOK)) {
        CANSTMOB &= ~(_BV(RXOK)); // reset receive interrupt flag
        read_msg();
    } else if (CANSTMOB & _BV(TXOK)) {
        CANSTMOB &= ~(_BV(TXOK)); // reset transmit interrupt flag
    } else {
        CANSTMOB &= 0; // unknown interrupt
    }

    read_msg();
    SREG=cSREG; //restore SREG
}
