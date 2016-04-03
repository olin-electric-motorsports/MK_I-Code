#define F_CPU 1000000L
#include <avr/io.h>
#include <util/delay.h>

int main(void){
    
    // Set pin 10 as output for testing LED
    DDRE |= _BV(PE1);

    // Declare variable fro analog data
    uint16_t analog_input;

    // Define ADC sample rate
    ADCSRA |= _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

    // Set ADC to reference internal ground
    ADCSRB |= _BV(AREFEN);

    // Set pin 14 as operational ADC
    ADMUX |= _BV(MUX0) | _BV(MUX1);

    // Read analog in and output through CAN FOREVER
   while(1){

       // Read ADC
        ADCSRA |= _BV(ADSC);

        // Wait for value to be read
        while(bit_is_set(ADCSRA, ADSC));
       
        // Record input value
        analog_input = ADC;

        // Set LED on when input is high, low when no
        if(analog_input > 512){
            PORTE |= _BV(PE1);
        }else{
            PORTE &= ~_BV(PE1);
        }
   }

}
