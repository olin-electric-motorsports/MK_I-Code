#define F_CPU (1000000L)
#include "api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int init_button(void){
    //EICRA = _BV(ISC00) | _BV(ISC30);
    EICRA |= _BV(ISC31);
    EIMSK |= _BV(INT3);

    return 0;
}

ISR(INT3_vect){

    PORTE |= _BV(PE1);

    uint8_t tmp;
    uint8_t msg[1];
    msg[0] = 0x03;
    CAN_Tx( send_demo, msg, 1 );
    _delay_ms(500);

    PORTE &= ~_BV(PE1);
}

ISR(CAN_INT_vect){
    PORTC |= _BV(PC4);
}


/*
ISR(INT1_vect){
    PORTC |= _BV(PC4);

    uint8_t msg[1];
    msg[0] = 0x08;

    if( CAN_send(0x20, MSG_demoMsg, msg, 1) < 0 ){
        PORTD |= _BV(PD7); // Turn on debug
    }

    // Instead of a delay, use a timer which
    // disables INT1 & then re-enables it
    _delay_ms(500);
    PORTC &= ~( _BV(PC4) );
}
*/


int main(void){
    sei();
    DDRE |= _BV(PE1);  // pin 10; Busy Light
    DDRC |= _BV(PC4);  // pin 17; Error Light
    DDRC &= ~_BV(PC0); // pin 30; Button input

    init_button();

    PORTE |= _BV(PE1);
    CAN_init();
    PORTE &= ~_BV(PE1);

    for(;;){
    }
}
