#define F_CPU (1000000L)
#include "api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

ISR(CAN_INT_vect){
    uint8_t mob = 0;
    uint8_t msg = 0;
    uint8_t i=0;
    for( mob = 0; mob < 5; mob++ ){
        if( (CANSIT2 && _BV(mob)) ){
            CANPAGE = (mob << 4);
            msg = CANMSG;
            break;
        }
    }

    for( i = 0; i < msg; i++){
        _delay_ms(500);
        PORTE |= _BV(PE1);
        _delay_ms(500);
        PORTE &= ~_BV(PE1);
    }

    CAN_Rx(IDT_demo, IDT_demo_l, IDM_single);
}


static int initADC( void ){
    //Enable ADC, set prescalar to 128 (slow down ADC clock)
    ADCSRA |= _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

    //Enable internal reference voltage
    ADCSRB &= _BV(AREFEN);

    //Set internal reference voltage as AVcc
    ADMUX |= _BV(REFS0);

    //Reads by default from ADC0 (pin 11); this line
    //  is redundant.
    ADMUX |= _BV( 0x00 );

    //No prescaling on PWM clock
    TCCR0B |= _BV(CS00);

    //Set up phase-correct PWM on OC0B
    TCCR0A |= _BV(COM0B1) | _BV(WGM00);

    //Reset the other PWM pin
    TCCR0A &= ~_BV(COM0B0);

    //set OC0B (PE1, pin 10) as output.
    DDRE |= _BV(PE1);
}


int main(void){
    sei();
    DDRE |= _BV(PE1);  // pin 10; Busy Light
    DDRC |= _BV(PC4);  // pin 17; Error Light
    DDRB |= _BV(PB2);

    CAN_init();
    CAN_Rx(IDT_demo, IDT_demo_l, IDM_single);

    for(;;){
        //Read from ADC
        ADCSRA |=  _BV(ADSC);
        //Wait for ADC reading
        while(bit_is_set(ADCSRA, ADSC));
        uint8_t reading = (uint8_t)(ADC>>2);
        CAN_Tx( IDT_watchdog, *reading, IDT_watchdog_l);
        _delay_ms(200);
    }
}
