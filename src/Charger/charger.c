#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main( void ){
    // Outputs
    DDRB |= _BV(PB5); // LED 1
    DDRB |= _BV(PB6); // LED 2
    DDRB |= _BV(PB7); // LED 3

    for(;;){
        PORTB ^= _BV(PB5) | _BV(PB6) | _BV(PB7);
        _delay_ms(200);
    }
}
