#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

ISR(CAN_INT_vect){
    //receives a can message containing the value of the amplitude. 
    //Will store this value right into OCR0A and OCR0B
    //by the way i have no idea if I am reading in can messages correctly
    uint8_t message = 0;
    if( bit_is_set(CANSIT2, 0)){ //mob0 is throttle
      CANPAGE = 0x00;
      CANPAGE = 0 << MOBNB0; // This doesn't make sense for MOb 0 but for others it does

      message = CANMSG;
    }
    //Set counters to the values specified by the can message
    OCR0B = message;

}

void initIO( void ){
    // Brake light
    DDRC |= _BV(PC7);

    // Brake sense
    DDRB |= _BV(PB7);

    // Left throttle
    DDRD |= _BV(PE1);

    // Right throttle
    DDRD |= _BV(PC1);

    // Forward switch
    DDRB |= _BV(PB2);
    DDRD |= _BV(PD7);

    // Onboard LED
    DDRC |= _BV(PC4);
}

//get can message from throttle
int main(void){
    //initializing things
    CAN_init(1);
    sei();


    //Enable ADC, set prescalar to 128 (slow down ADC clock)
    ADCSRA |= _BV(ADEN); //| _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

    //Enable internal reference voltage
    ADCSRB &= _BV(AREFEN);

    //Set internal reference voltage as AVcc
    ADMUX |= _BV(REFS0);

    //Reads by default from ADC0 (pin 11); this line
    //  is redundant.
    ADMUX |= _BV( 0x00 );
    
    /*blinky*/
    /*
    DDRC |= _BV(PC4);

    for(;;){
        PORTC ^= _BV(PC4);
        _delay_ms(500);
    }
    */

    /*pwm setup*/

    //need to set counters
    //OCR0B is for OC0B
    OCR0B = 0x00;

    //phase correct pwm
    //counter based on value in OCR0A and OCR0B (for pins OC0A and OC0B respectively)
    //setting WGM02:0 to 5 allows you to do phase correct pwm and set your own counter
    //Pretty sure count is the length of the duty cycle or something.
    //and it increases as you go up and decreases as you go down?
    //kinda fuzzy.
    TCCR0A |= _BV(WGM00);
    TCCR0A &= ~(_BV(WGM01));

    //COM0A0:1 controls the inversion of pwm output in OC0A
    //same for COM0B0:1 for OC0B
    //not sure what inversion does, but you have to choose one
    //I think it's probably something about defaulting to high power or low power at different duty cycles?
    TCCR0A |= _BV(COM0B1);
    TCCR0A &= ~(_BV(COM0B0));

    //prescaling: CS02:0
    //prescalar of 1 is a pwm frequency of ~1960 Hz
    //other prescalar values have smaller frequencies
    //not sure what frequency we need
    TCCR0B |= _BV(CS00);
    TCCR0B &= ~(_BV(CS02) | _BV(CS01));

    //pin setup
    DDRE |= _BV(PE1);

    /*16 bit timer
       OC1B*/

    //need to set counters
    //OCR0B is for OC0B
    OCR1BL = 0x00;

    //phase correct pwm
    //counter based on value in OCR0A and OCR0B (for pins OC0A and OC0B respectively)
    //setting WGM02:0 to 5 allows you to do phase correct pwm and set your own counter
    //Pretty sure count is the length of the duty cycle or something.
    //and it increases as you go up and decreases as you go down?
    //kinda fuzzy.
    TCCR1A |= _BV(WGM10);
    TCCR1A &= ~(_BV(WGM11));

    //COM0A0:1 controls the inversion of pwm output in OC0A
    //same for COM0B0:1 for OC0B
    //not sure what inversion does, but you have to choose one
    //I think it's probably something about defaulting to high power or low power at different duty cycles?
    TCCR1A |= _BV(COM1B1);
    TCCR1A &= ~(_BV(COM1B0));

    //prescaling: CS02:0
    //prescalar of 1 is a pwm frequency of ~1960 Hz
    //other prescalar values have smaller frequencies
    //not sure what frequency we need
    TCCR1B |= _BV(CS10);
    TCCR1B &= ~(_BV(CS12) | _BV(CS11));

    //pin setup
    DDRC |= _BV(PC1);
    
    //CANRX
    CAN_Rx(0, IDT_throttle, IDT_throttle_l, IDM_single);
    

}
