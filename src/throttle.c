#define F_CPU 1000000L
#include <avr/io.h>
#include <util/delay.h>


int main(void){

    // Set variables for throttle boolean and potentiometer readings
    int throttle_on = 0;
    uint16_t pot1;
    uint16_t pot2
    
    // Set sample rate for ADC
    ADCSRA |= _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);;
    
    // Set reference voltage for ADC as internal
    ADCSRB |= _BV(AREFEN);
    
    //Set PE1 as output (pin 10)
    DDRE |= _BV(PE1);
    
    while (1){
        
        if (throttle_on){
            PORTE |= _BV(PE1);
        }else{
            PORTE &= ~(_BV(PE1));
        }        
    }
}
