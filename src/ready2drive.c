#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <sys/time.h>

clock_t start;
//leave pins 6&7 for can
//button bouncing?
int main(void){
    //allows interruptions
    start = clock();
    sei();

    //Sets PE2 for output-pin11
    DDRE |= _BV(PE2);

    EICRA |= (_BV(ISC01)); // INT0 - Pin 14
    EICRA &= (~_BV(ISC00));
    EIMSK |= _BV(INT0);

    //don't do anything else, ever
    while(1);
}

ISR(INT0_vect){
    if((clock()-start)/CLOCKS_PER_SEC>.25){
        PORTE |= _BV(PE2);
        _delay_ms(2500);
        PORTE &= ~(_BV(PE2));
    }
    start = clock();
}
