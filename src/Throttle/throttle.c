#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>

uint8_t channels[] = {4, 7};

void handleBoot( void ){
    if( bit_is_set(MCUSR, WDRF) ){
        // Handle Watchdog reset
        MCUSR = 0x00;
        wdt_disable();

        PORTE |= _BV(PE1);

        //TODO: Send a CAN message
    } else {
        // Flash all LEDs
        PORTC |= _BV(PC5) | _BV(PC4);
        _delay_ms(1000);
        PORTC &= ~(_BV(PC5) | _BV(PC4));
    }
}

void initIO( void ){
    // Throttle Sensing
    DDRB &= ~_BV(PB6); // ADC7
    DDRB &= ~_BV(PB7); // ADC4

    // Brake
    DDRC &= ~_BV(PC0);

    // Onboard LEDs
    DDRC |= _BV(PC5);
    DDRC |= _BV(PC4);

    // LW speed
    DDRD &= ~_BV(PD4); // ADC1

    // RW speed
    DDRD &= ~_BV(PD5); // ADC2
}

void initADC( void ){
    // Enable ADC
    ADCSRA |= _BV(ADEN);
    
    // Setup prescaler ( 32 )
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

    //ADCSRB &= ~_BV(AREFEN);
    ADCSRB |= _BV(AREFEN);

    // Reference AVcc
    ADMUX |= _BV(REFS0);

    // Default to first input
    ADMUX |= channels[0];
}

void initTimer( void ){
    TCCR0A |= _BV(WGM01);

    TCCR0B |= _BV(CS02) | _BV(CS00);

    TIMSK0 |= _BV(OCIE0A);
    OCR0A = 100;
}


ISR(TIMER0_COMPA_vect){
    uint16_t thrott1;
    uint16_t thrott2;
    uint8_t msg[4];
    //PORTC |= _BV(PC5) | _BV(PC4);
    
    ADMUX &= ~(0x1F);
    ADMUX |= channels[0];
    ADCSRA |= _BV(ADSC);
    while(bit_is_set(ADCSRA, ADSC));
    thrott1 = ADC;
    
    ADMUX &= ~(0x1F);
    ADMUX |= channels[1];
    ADCSRA |= _BV(ADSC);
    while(bit_is_set(ADCSRA, ADSC));
    thrott2 = ADC;

    msg[0] = 0x00; // stnd
    msg[1] = PINC & _BV(PC0); // brake
    msg[2] = (uint8_t) (thrott1 >> 2);
    msg[3] = (uint8_t) (thrott2 >> 2);

    //loop_until_bit_is_clear(CANGSTA, TXBSY);
    CAN_Tx(0, IDT_THROTTLE, IDT_THROTTLE_L, msg);

    //PORTC &= ~(_BV(PC5) | _BV(PC4));

    //wdt_reset();
}

ISR(CAN_INT_vect){

}


int main(void){
    sei(); // Enable interrupts

    initIO();
    handleBoot();
    initTimer();
    initADC();

    //wdt_enable(WDTO_500MS);

    CAN_init(0);

    for(;;){
    }
}
