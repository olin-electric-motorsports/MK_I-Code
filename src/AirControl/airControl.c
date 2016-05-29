#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


void initIO( void ){
    // Onboard LEDs
    DDRB |= _BV(PB7) | _BV(PB6) | _BV(PB5);

    // Shutdown Sense
    DDRD &= ~_BV(PD5); // PCINT21

    // Shutdown circuit (???)
    DDRB |= _BV(PB0);
}

void initInterrupts(void){
    PCICR |= _BV(PCIE2);

    PCMSK2 |= _BV(PCINT21);
}

ISR(PCINT2_vect){

    if( bit_is_set(PIND, PD5)){
        PORTB |= _BV(PB7);
        _delay_ms(2500);
        PORTB |= _BV(PB6);
        PORTB |= _BV(PB0);
    } else {
        PORTB &= ~_BV(PB7);
        PORTB &= ~_BV(PB6);
        PORTB &= ~_BV(PB0);
    }
}

int main(void){
    sei();

    initIO();
    initInterrupts();
    //CAN_init(0, 0);
    
    for(;;){
        PORTB ^= _BV(PB5);
        _delay_ms(50);
        PORTB ^= _BV(PB5);
        _delay_ms(500);
    }
}
