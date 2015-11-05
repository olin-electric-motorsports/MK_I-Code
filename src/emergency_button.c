#include <avr/io.h>

int main(void){
    //allows interruptions
    sei();

    //Sets pin 10 for output
    //outputs when an emergency button is pressed
    //3 outputs
    //leds for now; will change when Byron figures out sensing system
    //estop1
    DDRE |= _BV(PE1);
    //estop2
    DDRE |= _BV(PE2);
    //estop3
    DDRD |= _BV(PD4);

    //Generates interrupt when certain pins (set below) are at 0V
    //Emergency buttons are normally closed; when they are pressed,
    //there will be no more voltage through the circuit
    EICRA = _BV(ISC00);

    //Sets pins 3, 6, and 7 to input
    //these pins can generate interruptions
    //each pin corresponds to an emergency stop
    //estop 1
    EIMSK |= _BV(INT0);
    //estop 2
    EIMSK |= _BV(INT1);
    //estop3
    EIMSK |= _BV(INT2);

    //Run forever.
    while(1);
}

//method to handle an interrupt from estop1
ISR(INT0_vect){
    //Send 5V to output pin PE1, drives an led
    PORTE |= _BV(PE1);
}

//method to handle an interrupt from estop12
ISR(INT1_vect){
    //Send 5V to output pin PE2, drives an led
    PORTE |= _BV(PE2);
}

//method to handle an interrupt from estop1
ISR(INT2_vect){
    //Send 5V to output pin PD4, drives an led
    PORTD |= _BV(PD4);
}
