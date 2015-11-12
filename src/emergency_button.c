#include <avr/io.h>
#include <avr/interrupt.h>

// INT0 has priority over INT1, which has priority over INT2, which I
// presume has priority over INT3, no matter the order you code it or wire it
// Need to ask anout pullup resistors for all switches
// Need to test 12V running through the circuit--find proper resistors.
int main(void){
    //allows interruptions
    sei();

    // Sets 3 pins for output
    // leds for now; will change when Byron figures out sensing system
    // estop1 - Pin 10
    DDRE |= _BV(PE1);
    // estop2 - Pin 28
    DDRB |= _BV(PB7);
    // estop3 - Pin 22
    DDRC |= _BV(PC6);

    // Generates interrupt when INT0, INT1, and INT2 are at 0V
    // Emergency buttons are normally closed; when they are pressed,
    // there will be no more voltage through the circuit
    EICRA &= ~(_BV(ISC00) | _BV(ISC01)); // INT0 - Pin 14
    EICRA &= ~(_BV(ISC10) | _BV(ISC11)); // INT1 - Pin 16
    EICRA &= ~(_BV(ISC20) | _BV(ISC21)); // INT2 - Pin 26

    // Sets INT0, INT1, and INT2 to input
    // These pins can generate interruptions
    // Each pin corresponds to an emergency stop
    // estop 1 - Pin 14
    EIMSK |= _BV(INT0);
    // estop 2 - Pin 16
    EIMSK |= _BV(INT1);
    // estop3 - Pin 26
    EIMSK |= _BV(INT2);

    //Run forever.
    while(1);
}

// Method to handle an interrupt from estop1
ISR(INT0_vect){
    // Send 5V to output pin PE1, drives an led
    PORTE |= _BV(PE1);
    // Turns off LED when it is done
    // The LED will stay on while this interrupt is being generated, for some reason
    PORTE &= ~(_BV(PE1));
}

// Method to handle an interrupt from estop2
ISR(INT1_vect){
    // Send 5V to output pin PE2, drives an led
    PORTB |= _BV(PB7);
    // Turns off LED when it is done
    // The LED will stay on while this interrupt is being generated, for some reason
    PORTB &= ~(_BV(PB7));
}

// Method to handle an interrupt from estop3
ISR(INT2_vect){
    // Send 5V to output pin PD4, drives an led
    PORTC |= _BV(PC6);
    // Turns off LED when it is done
    // The LED will stay on while this interrupt is being generated, for some reason
    PORTC &= ~(_BV(PC6));
}
