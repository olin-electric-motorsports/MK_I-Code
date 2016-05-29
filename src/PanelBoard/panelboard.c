#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

void initIO( void ){
    // Ready 2 Drive
    DDRB |= _BV(PB1);
    //PORTB |= _BV(PB1);

    // AMS reset button
    DDRD &= ~_BV(PD6);

    // IMD sense
    DDRC &= ~_BV(PC0);

    // Onboard LED
    DDRB |= _BV(PB7);

    // Shutdown activation
    DDRD |= _BV(PD5);

    // Button LEDs
    //PD0
    //PB6
    DDRD |= _BV(PD0);
    DDRB |= _BV(PB6);
    //PORTD |= _BV(PD0);
    //PORTB |= _BV(PB6);
}

int main( void ){
    sei();

    initIO();

    for(;;){
        if( bit_is_set(PINC, PC0)){
            PORTB |= _BV(PB6);
        } else {
            PORTB &= ~_BV(PB6);
        }
        
        if( bit_is_set(PIND, PD6) ){
            PORTB |= _BV(PB1);
        } else {
            PORTB &= ~_BV(PB1);
        }

        PORTB ^= _BV(PB7);
        //PORTB ^= _BV(PB1);
        _delay_ms(500);
    }
    return 1;
}

