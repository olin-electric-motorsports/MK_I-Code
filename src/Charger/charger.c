#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void initIO( void ){
    //IMD sense
    DDRB &= ~_BV(PB1); // PCINT1

    //LEDs
    DDRB |= _BV(PB5) | _BV(PB6) | _BV(PB7);
}

void initInterrupts( void ){
    PCICR |= _BV(PCIE0);

    PCMSK0 |= _BV(PCINT1);
}

void initTimer( void ){
    // 8-bit timer, CRC mode
    TCCR0A |= _BV(WGM01);

    // clk_IO/1024 prescaler
    // About 10ms per interrupt
    TCCR0B |= _BV(CS02) | _BV(CS00);

    // Interrupt on compare to OCR0A
    TIMSK0 |= _BV(OCIE0A);
    OCR0A = 100;
}

ISR(PCINT0_vect){
    uint8_t msg[] = { 0x00, 0x00 };
    PORTB |= _BV(PB5);

    if( bit_is_set(PINB, PB1) ){
        PORTB &= ~_BV(PB5);
    } else {
        PORTB |= _BV(PB5);
        msg[1] = 0x01;
        CAN_Tx( 0, IDT_CHARGER, IDT_CHARGER_L, msg );
    }
}

ISR(TIMER0_COMPA_vect){
    static uint8_t timerCounter;
    timerCounter++;
    uint8_t msg[] = { 0x00, 0x00 };

    PORTB |= _BV(PB5);
    if( timerCounter == 3 ){
        timerCounter = 0;
        if( bit_is_set(PINB, PB1) ){

        } else {
            msg[1] = 0x01;
            CAN_Tx( 0, IDT_CHARGER, IDT_CHARGER_L, msg );
        }
    }
    PORTB &= ~_BV(PB5);
}

int main( void ){
    sei();
    initIO();
    //initInterrupts();
    initTimer();
    CAN_init(0);

    for(;;){
        //_delay_ms(200);
    }
}
