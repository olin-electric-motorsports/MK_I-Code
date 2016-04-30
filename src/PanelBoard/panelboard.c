#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

void initIO( void ){
    // Ready 2 Drive
    DDRB |= _BV(PB1);

    // AMS reset button
    DDRD &= ~_BV(PD6);

    // IMD sense
    DDRC &= ~_BV(PC0);

    // Onboard LED
    DDRB |= _BV(PB7);

    // Shutdown activation
    DDRD |= _BV(PD5);
}

int main( void ){
    sei();

    initIO();

    for(;;){
    }
    return 1;
}
