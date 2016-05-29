#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>


/* Change this per BMS */
#define IDT_BMS IDT_BMS_1

/* MIN/MAX */
#define MAXV ((uint16_t) (4.15/5 * 0x3ff))
#define MINV ((uint16_t) (2.75/5 * 0x3ff))

// Max temperature
#define MAXTEMP ((uint16_t) (1.1/5 * 0x3ff))
#define MINTEMP ((uint16_t) (4.5/5 * 0x3ff))

// Status of BMS
#define NORMAL      ((uint8_t) 0 )
#define LOW_VOLTAGE ((uint8_t) 1 )
#define LOW_TEMP    ((uint8_t) 2 )
#define HIGH_TEMP   ((uint8_t) 3 )
#define CHARGING    ((uint8_t) 4 )

// Global constants
const uint8_t inputs[] = { 8, 5, 9, 3, 0, 2}; // 9 is breaking
const uint8_t temp[] = {6, 7, 4};
const uint8_t outputs[] = { _BV(PB3), _BV(PB4),
                            _BV(PC7), _BV(PD0),
                            _BV(PC0), _BV(PD1) };

// Global variables
uint8_t shunt[] = { 0, 0, 0, 0, 0, 0 };

//Standard/Shunt/temp1/temp2/temp3/tempInternal/Vavg/Vstd
//0x00/0x00~0x3f/0x00~0xff/0x00~0xff/0x00~0xff/0x00~0xff/0x00~0xff/0x00~0xff
uint8_t gMessage[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t global_status = NORMAL;

uint8_t gAutoReset = 0x00;

uint16_t readADC( uint8_t channel, uint8_t channel_next ){
    // Set channel if not set correctly beforehand
    if( !((ADMUX & 0x1F) == channel) ){
        ADMUX &= ~(0x1F);
        ADMUX |= channel;
    }

    // Que ADC reading
    ADCSRA |= _BV(ADSC);
    // Wait for ADC to finish
    while(bit_is_set(ADCSRA, ADSC));

    /*
    // Select next channel
    uint8_t nextChannel = (channel+1)%6;
    ADMUX |= inputs[nextChannel];
    */
    
    // Select next channel to maximize settling time
    // of the ADC
    ADMUX &= ~(0x1F);
    ADMUX |= channel_next;

    // Return reading
    return ADC;
}


void handleBoot( void ){
    if( bit_is_set(MCUSR, WDRF) ){
        // Handle Watchdog reset
        MCUSR = 0x00;
        wdt_disable();

        PORTE |= _BV(PE1);

        //TODO: Send a CAN message
    } else {
        // Flash all LEDs
        PORTE |= _BV(PE1);
        PORTD |= _BV(PD7);
        PORTB |= _BV(PB1);
        _delay_ms(100);
        PORTB &= ~_BV(PB1);
        PORTD &= ~_BV(PD7);
        PORTE &= ~_BV(PE1);
    }
}


void initIO( void ){
    // Debug lights 
    DDRE |= _BV(PE1); // Light 1 SHUNT
    DDRB |= _BV(PB1); // Light 2 ADC READING
    DDRD |= _BV(PD7); // Light 3 ERRORS

    // Outputs
    DDRB |= _BV(PB3); // Output 1
    DDRB |= _BV(PB4); // Output 2
    DDRC |= _BV(PC7); // Output 3
    DDRD |= _BV(PD0); // Output 4
    DDRC |= _BV(PC0); // Output 5
    DDRD |= _BV(PD1); // Output 6

    // Shutdown Circuit
    DDRB |= _BV(PB0);
    PORTB &= ~_BV(PB0);
    //PORTB |= _BV(PB0);// Don't enable by default

    // Disable Shunt
    PORTB &= ~( _BV(PB3) | _BV(PB4) );
    PORTC &= ~( _BV(PC7) | _BV(PC0) );
    PORTD &= ~( _BV(PD0) | _BV(PD1) );

    // Inputs 
    DDRC &= ~_BV(PC4); // Input 1, ADC8
    DDRB &= ~_BV(PB2); // Input 2, ADC5
    DDRC &= ~_BV(PC5); // Input 3, ADC9
    DDRD &= ~_BV(PD6); // Input 4, ADC3
    DDRE &= ~_BV(PE2); // Input 5, ADC0
    DDRD &= ~_BV(PD5); // Input 6, ADC2

    // Temperature
    //DDRC &= ~_BV(PC6); // Temp 1, ADC10
    DDRB &= ~_BV(PB5); // Temp 2, ADC6
    DDRB &= ~_BV(PB6); // Temp 3, ADC7
    DDRB &= ~_BV(PB7); // Temp 4, ADC4
}


void initADC( void ){
    // Enable ADC
    ADCSRA |= _BV(ADEN);
    
    // Setup prescaler ( 32 )
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

    //ADCSRB &= ~_BV(AREFEN);
    ADCSRB |= _BV(AREFEN);

    // Reference AVcc
    ADMUX |= _BV(REFS0);

    // Default to first input
    ADMUX |= inputs[0];
}


void initTimer( void ){
    // 8-bit timer, CRC mode
    TCCR0A |= _BV(WGM01);

    // clk_IO/1024 prescaler
    // About 10ms per interrupt
    TCCR0B |= _BV(CS02) | _BV(CS00);

    // Interrupt on compare to OCR0A
    TIMSK0 |= _BV(OCIE0A);
    OCR0A = 100;
}


void checkCellVoltages( void ){
    uint8_t ch;
    uint16_t voltage;

    // Check Voltage at cells
    for( ch =0; ch < 6; ch++ ){
        voltage = readADC(inputs[ch], inputs[((ch+1)%6)]);
        if( voltage >= MAXV ){
            shunt[ch] = 1;
            gMessage[1] |= _BV(ch);
        } else if( voltage <= MINV ){
            PORTB &= ~_BV(PB0);
            global_status = LOW_VOLTAGE;
            /* TODO: ENABLE
            CAN_Tx(MObError, IDT_BMS, IDT_BMS_l, ERR_UNDERVOLT);
            */
        } else {
            shunt[ch] = 0;
            gMessage[1] &= ~_BV(ch);
        }
    }
}


void checkTemperatures( void ){
    uint8_t ch;
    uint16_t voltage;
    uint8_t voltage_8bit;

    for( ch=0; ch < 3; ch++ ){
        voltage = readADC( temp[ch], temp[((ch+1)%4)] );
        voltage_8bit = (uint8_t )(voltage >> 2);
        gMessage[2+ch] = voltage_8bit;

        if( voltage <= MAXTEMP ){
            PORTB &= ~_BV(PB0);
            global_status = HIGH_TEMP;
        } else if (voltage >= MINTEMP ){
            PORTB &= ~_BV(PB0);
            global_status = LOW_TEMP;
        }
    }
}


void handleShunt( void ){
    uint8_t ch;
    for( ch = 0; ch < 6; ch++ ){
        if( shunt[ch] == 1 ){
            PORTE |= _BV(PE1); // Shunting
            if( ch == 0 || ch == 1 ) { PORTB |= outputs[ch]; }
            if( ch == 2 || ch == 4 ) { PORTC |= outputs[ch]; }
            if( ch == 3 || ch == 5 ) { PORTD |= outputs[ch]; }
        } else {
            if( ch == 0 || ch == 1 ) { PORTB &= ~(outputs[ch]); }
            if( ch == 2 || ch == 4 ) { PORTC &= ~(outputs[ch]); }
            if( ch == 3 || ch == 5 ) { PORTD &= ~(outputs[ch]); }
        }
    }
}


ISR(CAN_INT_vect){
    PORTE |= _BV(PE1);
    if( bit_is_set( CANSIT2, 1 )){
        CANSTMOB = 0x00;
        gAutoReset = 0;
        global_status = CHARGING;
        PORTB |= _BV(PB0);
    } else if ( bit_is_set( CANSIT2, 2)){
        CANSTMOB = 0x00;
        PORTB |= _BV(PB0);
       // CANPAGE = 2 << MOBNB0;
    }
    PORTE &= ~_BV(PE1);
            
    /*
    if( bit_is_set( CANSIT2, MObUpdate )){
        // Select correct MOb
        CANPAGE = MObUpdate << MOBNB0;

        // Check if message send success
        if( bit_is_set(CANSTMOB, TXOK) ){

            // Open up MOb; reset TXOK
            CANSTMOB = 0x00;
            return;
        }
    }

    // Don't handle any other interrupts
    // TODO: Auto-CAN restart
    */
}

void handleState( uint8_t timerCounter ){
    if( timerCounter % 2 == 0 ){
        PORTD &= ~_BV(PD7);
    } else if ( timerCounter < (2*global_status) ){
        PORTD |= _BV(PD7);
    }

    return;
}


ISR(TIMER0_COMPA_vect){
    static uint8_t timerCounter;
    timerCounter++;

    handleState(timerCounter);

    if( timerCounter == 3){
        if( global_status == CHARGING ){
            gAutoReset++;
            if( gAutoReset > 5 ){
                gAutoReset = 0;
                /*
                PORTB &= ~_BV(PB0);
                global_status = NORMAL;
                */
            }
        }
    } else if( timerCounter == 4){
        checkTemperatures();
    } else if( timerCounter == 9 ){

        // Let cells settls
        PORTB &= ~( _BV(PB3) | _BV(PB4) );
        PORTC &= ~( _BV(PC7) | _BV(PC0) );
        PORTD &= ~( _BV(PD0) | _BV(PD1) );
        
        // Shunt blinky
        PORTE &= ~_BV(PE1);

    } else if( timerCounter > 10 ){
        PORTB |= _BV(PB1); // State blinky

        timerCounter = 0;
        checkCellVoltages();
        handleShunt();

        PORTB &= ~_BV(PB1); // State blinky

        // Send CAN message on shunting status
        // Fails silently right now
        //CAN_Tx(0, IDT_BMS, IDT_BMS_L, gMessage);
    }


    // "Kick the dog"
    wdt_reset();
}


int main( void ){
    sei(); // enable interrupts

    initIO();
    handleBoot();
    initTimer();
    initADC();
    
    CAN_init(0, 1);
    CAN_Rx(1, IDT_CHARGER, IDT_CHARGER_L, IDM_single);
    //CAN_Rx(2, IDT_DASHBOARD, IDT_DASHBOARD_L, IDM_single);

    // Enable Watchdog timer
    // Timout after 500ms
    wdt_enable(WDTO_500MS);

    // Start up CAN
    //CAN_init(1);

    for(;;){
        /* Everything is handled by a timer */
    }
    return 1;
}

