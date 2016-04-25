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
    OCR0A = message;
    OCR0B = message;

}

//get can message from throttle
int main(void){
    //initializing things
    CAN_init(1);
    sei();

    
    /*blinky*/
    /*
    DDRC |= _BV(PC4);

    for(;;){
        PORTC ^= _BV(PC4);
        _delay_ms(500);
    }
    */

    /*pwm setup*/

    //pin setup
    DDRD |= _BV(OC0A);
    DDRE |= _BV(OC0B);

    //phase correct pwm
    //counter based on value in OCR0A and OCR0B (for pins OC0A and OC0B respectively)
    //setting WGM02:0 to 5 allows you to do phase correct pwm and set your own counter
    //Pretty sure count is the length of the duty cycle or something.
    //and it increases as you go up and decreases as you go down?
    //kinda fuzzy.
    TCCR0B |= _BV(WGM02);
    TCCR0A |= _BV(WGM00);
    TCCR0A &= ~(_BV(WGM01));

    //COM0A0:1 controls the inversion of pwm output in OC0A
    //same for COM0B0:1 for OC0B
    //not sure what inversion does, but you have to choose one
    //I think it's probably something about defaulting to high power or low power at different duty cycles?
    TCCR0A |= _BV(COM0A1);
    TCCR0A &= ~(_BV(COM0A0));
    TCCR0B |= _BV(COM0A1);
    TCCR0B &= ~(_BV(COM0A0));

    //prescaling: CS02:0
    //prescalar of 1 is a pwm frequency of ~1960 Hz
    //other prescalar values have smaller frequencies
    //not sure what frequency we need
    TCCR0B |= _BV(CS00);
    TCCR0B &= ~(_BV(CS02) & _BV(CS01));

    //need to set counters
    //OCR0A is for OC0A
    //OCR0B is for OC0B
    OCR0A = 0x00;
    OCR0B = 0x00;
    //currently set to nothing
    
    //CANRX
    CAN_Rx(0, IDT_throttle, IDT_throttle_l, IDM_single);
    


}
