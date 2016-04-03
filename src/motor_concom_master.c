#define F_CPU 1000000L
#include <avr/io.h>
#include <util/delay.h>
#include "inc/api.h"


int main(void){

    // Initialize CAN settings
    int CAN_init( void );

    // Set CAN message settings
    uint8_t msg[ IDT_demo_l ];
    msg[0] = 0xFF; // Set message to be sent as full-tilt
    
    // Set pin 10 as output for testing LED
    DDRE |= _BV(PE1);

    // Declare variable fro analog data
    uint8_t analog_input;

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
        if(analog_input > 768){
            CAN_Tx( IDT_demo, msg, IDT_demo_l ); // send CAN message
            PORTE |= _BV(PE1);
        }else{
            PORTE &= ~_BV(PE1);
        }
   }

}
