#define F_CPU 1000000L
#include <avr/io.h>
#include <util/delay.h>
#include "can_api.h"


int main(void){

    // Initialize CAN settings
    CAN_init( 0 );

    // Set CAN message settings
    uint8_t msg_low[ IDT_demo_l ];
    uint8_t msg_high[ IDT_demo_l ];
    msg_high[0] = 0xFF; // Set CAN message to turn on LED
    msg_low[0] = 0x00; // Set CAN message to turn off LED
    
    // Set pin 10 as output for testing LED
    DDRE |= _BV(PE1);

    // Declare variable for LED status
    uint8_t LED_status = 0;

    // Declare variable for analog data
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
        if(analog_input > 512 ){ // && LED_status == 0){
            LED_status = 1;
            PORTE |= _BV(PE1);
            CAN_Tx( 0, IDT_demo, IDT_demo_l, msg_high ); // send CAN message
        }
        if(analog_input <= 512 && LED_status == 1){
            LED_status = 0;
            CAN_Tx( 0, IDT_demo, IDT_demo_l, msg_low ); // send CAN message
            PORTE &= ~_BV(PE1);
        }
   }

}
