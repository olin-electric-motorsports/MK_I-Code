#include <avr/io.h>
#include <avr/interrupt.h>


int main(void){
    //allows interruptions
    sei();

    //Sets PE2 for output
    DDRE |= _BV(PE2);

    //Sets PE1 for input
    DDRE &= ~( _BV(PE1) );


    //while there is no input
    //pretty sure that boolean is super jank
    while(~(PINE & _BV(PE1)));

    //send 5V to output
    //drives the r2d relay for 2 seconds, then turns off
    PORTE |= _BV(PE2);
    _delay_ms(2000);
    PORTE &= ~(_BV(PE2));

    //don't do anything else, every
    while(1);
}
