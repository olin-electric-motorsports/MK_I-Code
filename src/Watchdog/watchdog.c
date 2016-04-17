#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

volatile uint8_t count_up = 0;

ISR(WDT_vect){
    PORTD |= _BV(PD7);
    _delay_ms(500);
    PORTD &= ~_BV(PD7);
    wdt_reset();
}

ISR(TIMER0_COMPA_vect){
    count_up++;
    if( count_up > 10 ){
        PORTE ^= _BV(PE1);
        count_up = 0;
    }
}

ISR(TIMER1_COMPA_vect){
    PORTE ^= _BV(PE1);
}


int main( void ){
    sei();
    // Debug lights 
    DDRE |= _BV(PE1); // Light 1
    DDRB |= _BV(PB1); // Light 2
    DDRD |= _BV(PD7); // Light 3

    if( bit_is_set(MCUSR, WDRF) ){
        // Handle Watchdog reset
        MCUSR = 0x00;
        wdt_disable();
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

    // Enable Watchdog timer
    wdt_enable(WDTO_500MS);

    // Enable timers 8 bit
    TCCR0A |= _BV(WGM01);
    TCCR0B |= _BV(CS02) | _BV(CS00);
    TIMSK0 |= _BV(OCIE0A);
    OCR0A = 100;
    

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
