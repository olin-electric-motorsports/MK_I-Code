#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "lcd.h"

void initIO(void){
    //Debug LEDs
    DDRB |= _BV(PB7) | _BV(PB6);

    //Pot input
    DDRC &= ~_BV(PC5); // ADC9

    //Button sensing
    DDRD &= ~_BV(PD5); // PCINT21  2
    DDRD &= ~_BV(PD6); // PCINT22  2

    // MultiSwitch
    DDRB &= ~_BV(PB1); // PCINT1   0
    DDRE &= ~_BV(PE1); // PCINT25  3
    DDRE &= ~_BV(PE2); // PCINT26  3

    // Switches
    DDRD &= ~_BV(PD3); // PCINT19  2
    DDRC &= ~_BV(PC6); // PCINT14  1
    DDRB &= ~_BV(PB3); // PCINT3   0
}

void initInterrupts(void){
    PCICR |= _BV(PCIE3) | _BV(PCIE2) | _BV(PCIE1) | _BV(PCIE0);

    PCMSK3 |= _BV(PCINT26) | _BV(PCINT25);
    PCMSK2 |= _BV(PCINT22) | _BV(PCINT21) | _BV(PCINT19);
    PCMSK1 |= _BV(PCINT14);
    PCMSK0 |= _BV(PCINT1) | _BV(PCINT3);
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
    ADMUX |= 9;
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
    uint8_t tmp;
    tmp = PINB;
    lcd_clrscr(); // TODO REMOVE
    if( bit_is_set(tmp, PB1) ){
        // TODO: Multiswitch 0
        lcd_puts("\nMultiswitch 0");
    } else if( bit_is_set(tmp, PB3) ){
        // TODO: Switch 2
        lcd_puts("\nSwitch 2");
    }
}

ISR(PCINT1_vect){
    uint8_t tmp;
    tmp = PINC;
    lcd_clrscr(); // TODO REMOVE
    if( bit_is_set(tmp, PC6) ){
        // TODO: Switch 1
        lcd_puts("\nSwitch 1");
    }
}

ISR(PCINT2_vect){
    uint8_t tmp;
    tmp = PIND;
    lcd_clrscr(); // TODO REMOVE
    if( bit_is_set(tmp, PD3) ){
        // TODO: Switch 0
        lcd_puts("\nSwitch 0");
    } else if( bit_is_set(tmp, PD5) ){
        // TODO: Button 0
        lcd_puts("\nButton 0");
    } else if( bit_is_set(tmp, PD6) ){
        // TODO: Button 1
        lcd_puts("\nButton 1");
    }
}

ISR(PCINT3_vect){
    uint8_t tmp;
    tmp = PINE;
    lcd_clrscr(); // TODO REMOVE
    if( bit_is_set(tmp, PE1) ){
        // TODO: Multiswitch 1
        lcd_puts("\nMultiswitch 1");
    } else if( bit_is_set(tmp, PE2) ){
        // TODO: Multiswitch 2
        lcd_puts("\nMultiswitch 2");
    }
}

ISR(TIMER0_COMPA_vect){
    lcd_clrscr();
    // Que ADC reading
    ADCSRA |= _BV(ADSC);
    // Wait for ADC to finish
    while(bit_is_set(ADCSRA, ADSC));

    uint16_t reading = ADC;
    char output[16];
    sprintf(output, "\n%d", reading);

    lcd_puts(output);
}

int main(void){
    DDRB |= _BV(PB7) | _BV(PB6);

    sei();
    //lcd_init(LCD_ON_DISPLAY);
    lcd_init(LCD_DISP_ON_CURSOR_BLINK);
    _delay_ms(100);
    lcd_clrscr();

    _delay_ms(100);
    
    lcd_puts("\nHello");
    initIO();
    //initInterrupts();
    initADC();
    initTimer();

    for(;;){
        //PORTB ^= _BV(PB7) | _BV(PB6);
        //_delay_ms(500);
    }
}
