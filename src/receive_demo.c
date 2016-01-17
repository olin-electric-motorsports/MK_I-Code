#define F_CPU (1000000L)
#include "api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

ISR(CAN_INT_vect){
    PORTE |= _BV(PE1);
    /*
    PORTE ^= _BV(PE1);
    _delay_ms(500);
    PORTE ^= _BV(PE1);
    _delay_ms(500);
    PORTE ^= _BV(PE1);
    */
}


int main(void){
    sei();
    DDRE |= _BV(PE1);  // pin 10; Busy Light
    DDRC |= _BV(PC4);  // pin 17; Error Light

    CAN_init();
    CAN_Rx(send_demo, 1);

    for(;;){
    }
}
