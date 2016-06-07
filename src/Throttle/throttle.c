#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>

uint8_t channels[] = {4, 7};

/* Keep State */
uint8_t rBrake       = 0x00;
uint16_t rThrottle[] = { 0x00, 0x00 };

void handleBoot(void)
{
    if (bit_is_set(MCUSR, WDRF))
    {
        // Handle Watchdog reset
        MCUSR = 0x00;
        wdt_disable();

        PORTE |= _BV(PE1);

        //TODO: Send a CAN message
    } 
    else 
    {
        // Flash all LEDs
        PORTC |= _BV(PC5) | _BV(PC4);
        _delay_ms(1000);
        PORTC &= ~(_BV(PC5) | _BV(PC4));
    }
}

void initIO(void)
{
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

void initInterrupts(void)
{
    PCICR  |= _BV(PCIE1);
    PCMSK1 |= _BV(PCINT8);
}

void initADC(void)
{
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

void initTimer(void)
{
    TCCR0A |= _BV(WGM01);

    TCCR0B |= _BV(CS02) | _BV(CS00);

    TIMSK0 |= _BV(OCIE0A);
    OCR0A = 100;
}


/*
uint8_t map(uint16_t low, uint16_t high, uint16_t read)
{
    uint16_t output = (uint16_t) (((float)(read - low) / (float)(high - low)) * 0xffff);
    return (uint8_t)( output >> 2 );
}
*/

uint16_t map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint8_t mapThrottle(void)
{
    uint8_t thrott1 = rThrottle[0] >> 2;
    uint16_t thrott2 = map(thrott1, 101, 115, 0, 255);
    return (uint8_t)(thrott2 >> 2);
}

void updateThrottle(void)
{
    uint8_t i;
    for (i = 0; i < 2; i++)
    {
        ADMUX &= ~(0x1F);
        ADMUX |= channels[i];
        ADCSRA |= _BV(ADSC);
        while(bit_is_set(ADCSRA, ADSC));
        rThrottle[i] = ADC;
    }
}

void checkThrottlePlausibility(void)
{
    // TODO: Implement
}


ISR(PCINT1_vect)
{
    uint8_t tmp = PINC;

    /* Brake */
    if (bit_is_set(tmp, PC0))
    {
        rBrake = 0x01;
        PORTC |= _BV(PC5) | _BV(PC4);
    }
    else
    {
        rBrake = 0x00;
        PORTC &= ~(_BV(PC5) | _BV(PC4));
    }
}


ISR(TIMER0_COMPA_vect)
{
   // PORTC |= _BV(PC5) | _BV(PC4);
    
    updateThrottle();
    checkThrottlePlausibility();

    uint8_t throttle = mapThrottle();
    
    uint8_t msg[] = { 0x00, 
                      bit_is_set(PINC, PC0), 
                      //map(100, 115, rThrottle[0]), 
                      //map(100, 115, rThrottle[1])
                      //map(rThrottle[0], 410, 460, 0, 255),
                      //map(rThrottle[1], 410, 460, 0, 255)
                      throttle,
                      throttle
                    };

    //loop_until_bit_is_clear(CANGSTA, TXBSY);
    CAN_Tx(0, IDT_THROTTLE, IDT_THROTTLE_L, msg);

    // PORTC &= ~(_BV(PC5) | _BV(PC4));
}

ISR(CAN_INT_vect)
{

}


int main(void){
    sei(); // Enable interrupts

    initIO();
    handleBoot();
    initTimer();
    initADC();
    initInterrupts();

    //wdt_enable(WDTO_500MS);

    CAN_init(0, 0);

    for(;;){
    }
}
