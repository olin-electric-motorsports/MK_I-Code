#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

ISR(WDT_vect){
    PORTD |= _BV(PD7);
    _delay_ms(500);
    PORTD &= ~_BV(PD7);
    wdt_reset();
}


int main( void ){
    // Debug lights 
    DDRE |= _BV(PE1); // Light 1
    DDRB |= _BV(PB1); // Light 2
    DDRD |= _BV(PD7); // Light 3
    DDRE |= _BV(PE0); // RESET

    if( bit_is_set(MCUSR, WDRF) ){
        MCUSR = 0x00;
        wdt_disable();
        PORTE &= ~_BV(PE0);
        PORTE |= _BV(PE1);
    } else {
        PORTE |= _BV(PE1);
        PORTD |= _BV(PD7);
        PORTB |= _BV(PB1);
    }
    _delay_ms(1500);
    PORTB &= ~_BV(PB1);
    PORTD &= ~_BV(PD7);
    PORTE &= ~_BV(PE1);

    // Enablge Watchdog timer
    wdt_enable(WDTO_500MS);

    for(;;){
        if( bit_is_set(WDTCSR, WDE) ){
            PORTB |= _BV(PB1);
        } else {
            PORTB &= ~_BV(PB1);
        }
        _delay_ms(450);
        wdt_reset();
    }
}
