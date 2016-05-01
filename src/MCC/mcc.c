#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

ISR(CAN_INT_vect){
    //receives a can message containing the value of the amplitude
    uint8_t message = 0;

    //cansit2 0:5 contains all the mobs
    //bit_is_set : checks if that index of that register is 1
    if( bit_is_set(CANSIT2, 0)){ //mob0 is throttle
        CANPAGE = 0x00; //clear canpage
        CANPAGE = 0 << MOBNB0; // bitshift the mob number you want to CANPAGE 7:4, which is MOBNB3:0 
        message = CANMSG; //get message
        //Set counters to the values specified by the can message
        OCR0B = message;
        OCR1BL = message;

        /***mob specific errors***/
        //receiving errors
        if( bit_is_set(CANSTMOB, RXOK) ){
            //everything is ok; reset this register
            CANSTMOB = 0x00;
            return;
        } else if(bit_is_set(CANSTMOB, DLCW)){
            //length of data is not what you expected
            log_error(5);
        } else if(bit_is_set(CANSTMOB, SERR)){
            //more than 5 consecutive dominant bits; going out of sync
            log_error(4);
        } else if(bit_is_set(CANSTMOB, CERR)){
            //checksum doesn't match up, message wasn't sent correctly
            log_error(2);
        } else if(bit_is_set(CANSTMOB, FERR)){
            //checksum doesn't match up, message wasn't sent correctly
            log_error(16);
        }

        /*
        //transmission errors
        //not relevant since mcc is not transmitting
        if( bit_is_set(CANSTMOB, TXOK) ){
            //everything is ok; reset this register
            CANSTMOB = 0x00;
            return;
        } else if(bit_is_set(CANSTMOB, AERR)){
            //message not acknowledged by reciever
            log_error(7);
            return;
        } else if(bit_is_set(CANSTMOB, BERR)){
            //the bits being monitored on can are different than the bits sent
            //oh nooo
            abort_CAN();
            return;
        } else {
            //transmission had other booboos
            log_error(6);
        }
        */
    }

    //general can eroors
    if( bit_is_set(CANGIT, AERG)){
        //acknowledgment error
        log_error(3);
    } else if (bit_is_set(CANGIT, BOFFIT)){
        //entered bus off mode; no error frames will be sent
        log_error(8);
    } else if (bit_is_set(CANGIT, OVRTIM)){
        //timer overrun interrupt--when timer goes from 0xFFFF to 0
        log_error(9);
    } else if (bit_is_set(CANGIT, BXOK)){
        //something in frame buffer mode did a good. who knows what it is. probably byron knows
        log_error(10);
    } else if (bit_is_set(CANGIT, SERG)){
        //stuff error interrupt--more than 5 consecutive dominant bits
        log_error(11);
    } else if (bit_is_set(CANGIT, CERG)){
        //checksum error interrupt, message hashes don't match up
        log_error(12);
    } else if (bit_is_set(CANGIT, FERG)){
        //form error, messages in the wrong form
        log_error(13);
    }
}

void log_error( uint8_t msg ){
    uint8_t i;
    for(;;){
        //blink msg amount of times
        for(i=0; i < msg; i++){
            PORTC |= _BV(PC4);
            _delay_ms(200);
            PORTC &= ~_BV(PC4);
            _delay_ms(200);
        }
        _delay_ms(1000);
    }
}

void abort_CAN( void ){
    //assuming we want to turn on the one led we have
    PORTC |= _BV(PC4);
    //submit an abort request -- reset canen1 and canen2
    CANGCON |= _BV(ABRQ);
    //wait for canen2 to be reset
    while(bit_is_set(CANEN2, 3)){}
    //undo that abort request
    CANGCON &= ~_BV(ABRQ);
    //can aborted
    PORTC &= ~_BV(PC4);
}

//get can message from throttle
int main(void){
    /***initializing things***/
    CAN_init(0);
    sei();
    //initialize led for errors
    DDRC |= _BV(PC4);

    //Enable ADC, set prescalar to 128 (slow down ADC clock)
    ADCSRA |= _BV(ADEN); //| _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

    //Enable internal reference voltage
    ADCSRB &= _BV(AREFEN);

    //Set internal reference voltage as AVcc
    ADMUX |= _BV(REFS0);

    //Reads by default from ADC0 (pin 11); this line
    //  is redundant.
    ADMUX |= _BV( 0x00 );

    /***pwm setup***/

    /**8-bit timer
        OC0B**/
    //need to set counters
    //OCR0B is for OC0B
    OCR0B = 0x00;

    //fast pwm
    //counter based on value in  OCR0B (for pin OC0B)
    //setting WGM02:0 to 3 allows you to do phase correct pwm and set your own counter
    //the data sheet lies
    TCCR0A |= _BV(WGM00);
    TCCR0A |= _BV(WGM01);

    //COM0B0:1 controls the inversion of pwm output in OC0B
    //not sure what inversion does, but you have to choose one
    TCCR0A |= _BV(COM0B1);
    TCCR0A &= ~(_BV(COM0B0));

    //prescaling: CS02:0
    //not sure what frequency we need
    TCCR0B |= _BV(CS00);
    TCCR0B &= ~(_BV(CS02) | _BV(CS01));

    //pin setup
    DDRE |= _BV(PE1);

    /**16 bit timer
       OC1B**/

    //need to set counters
    //Only need the low part of OCR1B
    OCR1BL = 0x00;

    //8-bit fast pwm
    //again, the data sheet lies
    TCCR1A |= _BV(WGM10);
    TCCR1B |= _BV(WGM12);

    //more inversion
    TCCR1A |= _BV(COM1B1);
    TCCR1A &= ~(_BV(COM1B0));

    //prescaling: CS02:0
    TCCR1B |= _BV(CS10);
    TCCR1B &= ~(_BV(CS12) | _BV(CS11));

    //pin setup
    DDRC |= _BV(PC1);
    
    /***CANRX***/
    CAN_Rx(0, IDT_throttle, IDT_throttle_l, IDM_single);
    

}
