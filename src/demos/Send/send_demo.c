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
    static uint8_t swap;

    uint8_t tmp;
    uint8_t msg[1];
    msg[0] = 0x03;
    if( swap == 0 ){
        tmp = CAN_Tx( IDT_demo, msg, IDT_demo_l );
        swap++;
    } else {
        tmp = CAN_Tx( IDT_watchdog, msg, IDT_watchdog_l );
        swap--;
    }

    if( tmp < 0 ){
        PORTB |= _BV(PB2);
    }
    _delay_ms(500);
}

ISR(CAN_INT_vect){
    PORTB |= _BV(PB2);
}

int main(void){
    sei();
    DDRE |= _BV(PE1);  // pin 10; Busy Light
    DDRB |= _BV(PB2);  // pin 16; Error Light
    DDRC &= ~_BV(PC0); // pin 30; Button input

    init_button();
    CAN_init();

    while( (CANGSTA & ENFG) != _BV(ENFG)){
    }

    for(;;){
    }
}
