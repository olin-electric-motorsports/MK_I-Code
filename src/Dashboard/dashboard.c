#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "lcd.h"

/*
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

ISR(PCINT0_vect){
    uint8_t tmp;
    tmp = PINB;
    lcd_clrscr(); // TODO REMOVE
    if( bit_is_set(tmp, PB1) ){
        // TODO: Multiswitch 0
        lcd_puts("Multiswitch 0\n");
    } else if( bit_is_set(tmp, PB3) ){
        // TODO: Switch 2
        lcd_puts("Switch 2\n");
    }
}

ISR(PCINT1_vect){
    uint8_t tmp;
    tmp = PINC;
    lcd_clrscr(); // TODO REMOVE
    if( bit_is_set(tmp, PC6) ){
        // TODO: Switch 1
        lcd_puts("Switch 1\n");
    }
}

ISR(PCINT2_vect){
    uint8_t tmp;
    tmp = PIND;
    lcd_clrscr(); // TODO REMOVE
    if( bit_is_set(tmp, PD3) ){
        // TODO: Switch 0
        lcd_puts("Switch 0\n");
    } else if( bit_is_set(tmp, PD5) ){
        // TODO: Button 0
        lcd_puts("Button 0\n");
    } else if( bit_is_set(tmp, PD6) ){
        // TODO: Button 1
        lcd_puts("Button 1\n");
    }
}

ISR(PCINT3_vect){
    uint8_t tmp;
    tmp = PINE;
    lcd_clrscr(); // TODO REMOVE
    if( bit_is_set(tmp, PE1) ){
        // TODO: Multiswitch 1
        lcd_puts("Multiswitch 1\n");
    } else if( bit_is_set(tmp, PE2) ){
        // TODO: Multiswitch 2
        lcd_puts("Multiswitch 2\n");
    }
}
*/

int main(void){
    DDRB |= _BV(PB7) | _BV(PB6);

    sei();
    //lcd_init(LCD_ON_DISPLAY);
    lcd_init(LCD_DISP_ON_CURSOR_BLINK);
    _delay_ms(1000);
    lcd_clrscr();

    _delay_ms(1000);
    
    lcd_puts("Hello");

    for(;;){
        PORTB ^= _BV(PB7) | _BV(PB6);
        _delay_ms(500);
    }
}
