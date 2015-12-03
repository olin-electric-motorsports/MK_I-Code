#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main(void){
    //allows interruptions
    sei();

    //Sets PE2 for output
    DDRE |= _BV(PE2);

    EICRA |= (_BV(ISC00) | _BV(ISC01)); // INT0 - Pin 14
    EIMSK |= _BV(INT0);

    //don't do anything else, ever
    while(1);
}

ISR(INT0_vect){
    PORTE |= _BV(PE2);
    _delay_ms(2000);
    PORTE &= ~(_BV(PE2));
}
