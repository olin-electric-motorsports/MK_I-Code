#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define PROGRAMMING 0
#define NORMAL      1

uint8_t gMode = 0x00;

void initIO(void)
{
    // Onboard LEDs
    DDRB |= _BV(PB7) | _BV(PB6) | _BV(PB5);

    // Shutdown Sense
    DDRD &= ~_BV(PD5); // PCINT21

    // High Side Air
    DDRB |= _BV(PB0);
}


void initInterrupts(void)
{
    PCICR |= _BV(PCIE2);

    PCMSK2 |= _BV(PCINT21);
}


void initTimer(void)
{
    // 8-bit timer, CRC mode
    TCCR0A |= _BV(WGM01);

    // clk_IO/1024 prescaler
    // About 10ms per interrupt
    TCCR0B |= _BV(CS02) | _BV(CS00);

    // Interrupt on compare to OCR0A
    TIMSK0 |= _BV(OCIE0A);
    OCR0A = 100;
}


ISR(PCINT2_vect)
{
    if (gMode == 0x01)
    {
        if (bit_is_set(PIND, PD5))
        {
            PORTB |= _BV(PB7);
            _delay_ms(2000);
            PORTB |= _BV(PB6);
            PORTB |= _BV(PB0);
        }
        else
        {
            PORTB &= ~_BV(PB7);
            PORTB &= ~_BV(PB6);
            PORTB &= ~_BV(PB0);
        }
    }
}


ISR(TIMER0_COMPA_vect)
{
    if (bit_is_set(PIND, PD5) )
    {
    }
    else 
    {
            PORTB &= ~_BV(PB7);
            PORTB &= ~_BV(PB6);
            PORTB &= ~_BV(PB0);
    }
    /* Should be an interrupt every 1/10th
     * of a second */
    
}


ISR(CAN_INT_vect)
{
    volatile uint8_t msg;

    if (bit_is_set(CANSIT2, 0))
    {
        CANPAGE = 0x00;
        CANPAGE = 0 << MOBNB0;

        msg = CANMSG;
        msg = CANMSG;

        gMode = msg;

        CANSTMOB = 0x00;
        loop_until_bit_is_clear(CANEN2, 0);

        CAN_Rx(0, IDT_DASHBOARD, IDT_DASHBOARD_L, IDM_single);
    }
}


int main(void)
{
    sei();

    initIO();
    initInterrupts();
    initTimer();
    CAN_init(0, 1);
    CAN_Rx(0, IDT_DASHBOARD, IDT_DASHBOARD_L, IDM_single);
    
    for(;;){
        PORTB ^= _BV(PB5);
        _delay_ms(50);
        PORTB ^= _BV(PB5);
        _delay_ms(500);
    }
}
