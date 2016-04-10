#define F_CPU 1000000L #include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "can_api.h"

ISR(CAN_INT_vect){

}

int main(void){

    // Enable global interrupts
    sei();

    // Initial CAN settings
    int CAN_init( void );
    
    // Set pin 10 as output for testing LED
    DDRE |= _BV(PE1);

    // Declare variable for CAN data
    uint8_t CAN_input;

    if( bit_is_set(CANSIT2, 0)){
         CANPAGE = 0x00;
         CANPAGE = 0 << MOBNB0;
 
         CAN_input = CANMSG;
    }
     // Set LED on when input is high, low when no
     if(CAN_input == 0xFF){
         PORTE |= _BV(PE1);
         CAN_input = 0x00;
     }else{
         PORTE &= ~_BV(PE1);
     }
}
