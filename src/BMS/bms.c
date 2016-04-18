#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#define MAXV ((uint16_t) (4.2/5.0 * 0x3ff))
#define MINV ((uint16_t) (3.0/5.0 * 0x3ff))

// Global constants
const uint8_t inputs[] = { 8, 5, 9, 3, 0, 2};
const uint8_t temp[] = {10, 6, 7, 4};
const uint8_t outputs[] = { _BV(PB3), _BV(PB4),
                            _BV(PC7), _BV(PD0),
                            _BV(PC0), _BV(PD1) };

// Global variable
uint8_t shunt[] = { 0, 0, 0, 0, 0, 0 };


uint16_t readADC( uint8_t channel ){
    // Que ADC reading
    ADCSRA |= _BV(ADSC);
    // Wait for ADC to finish
    while(bit_is_set(ADCSRA, ADSC));

    // Reset multiplexer bits
    ADMUX &= ~(0x1F);
    // Select next channel
    uint8_t nextChannel = (channel+1)%6;
    ADMUX |= inputs[nextChannel];

    // Return reading
    return ADC;
}


void handleBoot( void ){
    if( bit_is_set(MCUSR, WDRF) ){
        // Handle Watchdog reset
        MCUSR = 0x00;
        wdt_disable();

        PORTE |= _BV(PE1);

        //TODO: Send a CAN message
    } else {
        // Flash all LEDs
        PORTE |= _BV(PE1);
        PORTD |= _BV(PD7);
        PORTB |= _BV(PB1);
        _delay_ms(100);
        PORTB &= ~_BV(PB1);
        PORTD &= ~_BV(PD7);
        PORTE &= ~_BV(PE1);
    }
}


void initIO( void ){
    // Debug lights 
    DDRE |= _BV(PE1); // Light 1
    DDRB |= _BV(PB1); // Light 2
    DDRD |= _BV(PD7); // Light 3

    // Outputs
    DDRB |= _BV(PB3); // Output 1
    DDRB |= _BV(PB4); // Output 2
    DDRC |= _BV(PC7); // Output 3
    DDRD |= _BV(PD0); // Output 4
    DDRC |= _BV(PC0); // Output 5
    DDRD |= _BV(PD1); // Output 6

    // Disable Shunt
    PORTB &= ~( _BV(PB3) | _BV(PB4) );
    PORTC &= ~( _BV(PC7) | _BV(PC0) );
    PORTD &= ~( _BV(PD0) | _BV(PD1) );

    // Inputs 
    DDRC &= ~_BV(PC4); // Input 1, ADC8
    DDRB &= ~_BV(PB2); // Input 2, ADC5
    DDRC &= ~_BV(PC5); // Input 3, ADC9
    DDRD &= ~_BV(PD6); // Input 4, ADC3
    DDRE &= ~_BV(PE2); // Input 5, ADC0
    DDRD &= ~_BV(PD5); // Input 6, ADC2

    // Temperature
    DDRC &= ~_BV(PC6); // Temp 1, ADC10
    DDRB &= ~_BV(PB5); // Temp 2, ADC6
    DDRB &= ~_BV(PB6); // Temp 3, ADC7
    DDRB &= ~_BV(PB7); // Temp 4, ADC4
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
    ADMUX |= inputs[0];
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


void checkCellVoltages( void ){
    uint8_t ch;
    uint16_t voltage;

    // Check Voltage at cells
    for( ch =0; ch < 6; ch++ ){
        voltage = readADC(ch);
        if( voltage >= MAXV ){
            shunt[ch] = 1;
        } else {
            shunt[ch] = 0;
        }
    }
}


void handleShunt( void ){
    uint8_t ch;
    for( ch = 0; ch < 6; ch++ ){
        if( shunt[ch] == 1 ){
            PORTE |= _BV(PE1); // Shunting
            if( ch == 0 || ch == 1 ) { PORTB |= outputs[ch]; }
            if( ch == 2 || ch == 4 ) { PORTC |= outputs[ch]; }
            if( ch == 3 || ch == 5 ) { PORTD |= outputs[ch]; }
        } else {
            if( ch == 0 || ch == 1 ) { PORTB &= ~(outputs[ch]); }
            if( ch == 2 || ch == 4 ) { PORTC &= ~(outputs[ch]); }
            if( ch == 3 || ch == 5 ) { PORTD &= ~(outputs[ch]); }
        }
    }
}


ISR(TIMER0_COMPA_vect){
    static uint8_t timerCounter;
    //PORTE ^= _BV(PE1);
    timerCounter++;
    if( timerCounter == 9 ){
        // Let cells settls
        PORTB &= ~( _BV(PB3) | _BV(PB4) );
        PORTC &= ~( _BV(PC7) | _BV(PC0) );
        PORTD &= ~( _BV(PD0) | _BV(PD1) );

        PORTD ^= _BV(PD7); // State blinky
        PORTE &= ~_BV(PE1);// Shunt-detect blinky
    } else if( timerCounter > 10 ){
        PORTB ^= _BV(PB1); // State blinky

        timerCounter = 0;
        checkCellVoltages();
        handleShunt();

        //TODO: Send a CAN Message with updates
        //      on shunting status
    }

    // "Kick the dog"
    wdt_reset();
}


int main( void ){
    sei(); // enable interrupts

    initIO();
    handleBoot();
    initTimer();
    initADC();

    // Enable Watchdog timer
    // Timout after 500ms
    wdt_enable(WDTO_500MS);

    for(;;){
        /* Everything is handled by a timer */
    }
    return 1;
}


