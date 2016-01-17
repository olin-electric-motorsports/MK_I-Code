#include "api.h"

/* ************** *
 * Functions      *
 * ************** */
int CAN_init(){
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

    // Set up as Enabled mode
    //  instead of standby
    //  Necessary in order to get CAN
    //  communication
    CANGCON |= _BV( ENASTB );

    // This will be an infinite loop
    // if the CAN bus is stuck HIGH
    while( !(CANGSTA & _BV(ENFG))){
    }


    return(0);
}

/* ************** *
 * MOb Type Setup *
 * ************** */
int CAN_Tx(uint8_t nodeID, uint8_t msg[], uint8_t msg_length){
    // Select a MOb that is not being used
    uint8_t mob=0;
    for( mob = 0; mob < 5; mob++ ){
        if( !(CANEN2 && 1 << mob) ){
            break;
        }
    }
    if( mob == 5 ){
        return -1;
    }

    // Select CAN mob based on input MOb
    CANPAGE = (mob << MOBNB0);

    // Reset CANPAGE.INDXn
    CANPAGE &= ~(_BV(INDX0) | _BV(INDX1) | _BV(INDX2));

    // Clean CAN status for this MOb
    CANSTMOB = 0x0;

    // Set MOb ID
    CANIDT1 = ((nodeID & 0x1F) << 3); // node ID
    CANIDT2 = 0x00;
    CANIDT3 = 0x00;
    CANIDT4 = 0x00; // Data frame


    // Set mask to 0x00
    // Not used by Tx but good practice
    CANIDM1 = 0x00; 
    CANIDM2 = 0x00;
    CANIDM3 = 0x00;
    CANIDM4 = 0x00;

    // Set the message
    uint8_t i;
    for(i=0; i < msg_length; i++){
        CANMSG = msg[i];
    }
    
    // Send the message
    //CANCDMOB = _BV(CONMOB0) | (msg_length << DLC0);
    CANCDMOB = 0x00;
    CANCDMOB = (0x01 << CONMOB0) | (msg_length << DLC0);

    // Check for errors 
    // TODO: Set up interrupts for this shit
    while( (CANSTMOB & _BV(TXOK)) == 0){
        //if( CANSTMOB & _BV(BERR) != 0){
        //if( CANSTMOB & _BV(SERR) != 0){
        //if( CANSTMOB & _BV(CERR) != 0){
        //if( CANSTMOB & _BV(FERR) != 0){
        //if( CANSTMOB & _BV(AERR) != 0){
        if( CANGSTA & _BV(TXBSY) != 0){
            PORTC |= _BV(PC4);
    
        }else{
            PORTC &= ~_BV(PC4);
        }
    }

    // Should clear CANSTMOB once
    // Tx job is done
    CANSTMOB=0x00;

    return 0;
}


int CAN_Rx(uint8_t nodeID, uint8_t msg_length){
    // Select a MOb that is not being used
    uint8_t mob = 0;
    for( mob = 0; mob < 5; mob++ ){
        if( !(CANEN2 && 1 << mob) ){
            break;
        }
    }
    if( mob == 5 ){
        return -1;
    }

    // Select CAN mob based on input MOb
    CANPAGE = (mob << MOBNB0);

    // Reset CANPAGE.INDXn
    CANPAGE &= ~(_BV(INDX0) | _BV(INDX1) | _BV(INDX2));

    // Clean CAN status for this MOb
    CANSTMOB = 0x0;

    // Set MOb ID
    CANIDT1 = ((nodeID & 0x1F) << 3); // node ID
    CANIDT2 = 0x00;
    CANIDT3 = 0x00;
    CANIDT4 = 0x00; // Data frame

    // Set up MASK
    CANIDM1 = 0x00; //This node will recieve all messages
    CANIDM2 = 0x00;
    CANIDM3 = 0x00;
    //CANIDM4 = 0x00; // Ignore what is set above
    CANIDM4 = (_BV(RTRMSK) | _BV(IDEMSK)); // Use what is set above

    // Begin waiting for Rx
    //CANCDMOB = _BV(CONMOB1) | (msg_length << DLC0);
    CANCDMOB = 0x00;
    CANCDMOB = (0x02 << CONMOB0) | (msg_length << DLC0);
    

    // Should be caught by an interrupt and handled there
    /*
    while( !(CANSTMOB & _BV(RXOK)) ){
        PORTE |= _BV(PE1);
    }
    CANSTMOB=0x00;
    */

    return 0;
}
