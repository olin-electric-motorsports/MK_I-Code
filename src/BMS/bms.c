#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define MAXV ((uint16_t) (4.22/5.0 * 0x3ff))
#define MINV ((uint16_t) (3.0/5.0 * 0x3ff))
#define CAPTURE_STEP 1000

const uint8_t inputs[] = { 8, 5, 9, 3, 0, 2};
const uint8_t temp[] = {10, 6, 7, 4};
const uint8_t outputs[] = { _BV(PB3), _BV(PB4),
                            _BV(PC7), _BV(PD0),
                            _BV(PC0), _BV(PD1) };

volatile uint32_t millis = 0;
volatile uint8_t timer1_counter = 0;
uint32_t capture_time = 0;


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
    ADCSRA |= _BV(ADPS2) | _BV(ADPS0);

    // Reference AVcc
    ADMUX |= _BV(REFS0);
}

void initTimer( void ){
    TCCR1B |= ( _BV(WGM12) | _BV(CS10) );
    TIMSK1 |= _BV(OCIE1A);
    OCR1A = 248;
}

ISR(TIMER1_COMPA_vect){
    timer1_counter++;
    if (timer1_counter >=4){
        millis++;
        timer1_counter = 0;
    }
}

uint16_t readADC( uint8_t channel ){
    // Reset multiplexer bits
    ADMUX &= ~(0x1F);

    ADMUX |= inputs[channel];

    ADCSRA |= _BV(ADSC);
    while(bit_is_set(ADCSRA, ADSC));

    return ADC;
}

int main( void ){
    uint8_t shunt[] = { 0, 0, 0, 0, 0, 0 };
    initTimer();
    initADC();
    initIO();

    uint8_t ch;
    uint16_t voltage;

    for(;;){
        if( (millis - capture_time) > CAPTURE_STEP ){
            PORTE ^= _BV(PE1);
            capture_time += CAPTURE_STEP;

            // Let cells settls
            PORTB &= ~( _BV(PB3) | _BV(PB4) );
            PORTC &= ~( _BV(PC7) | _BV(PC0) );
            PORTD &= ~( _BV(PD0) | _BV(PD1) );
            _delay_ms(10);

            // Check Voltage at cells
            for( ch =0; ch < 6; ch++ ){
                voltage = readADC(ch);
                if( voltage >= MAXV ){
                    shunt[ch] = 1;
                } else {
                    shunt[ch] = 0;
                }
            }

            for( ch = 0; ch < 6; ch++ ){
                if( shunt[ch] == 1 ){
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
    }
    return 1;
}


