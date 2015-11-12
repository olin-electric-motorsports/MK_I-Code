#include "api.h"

#define NAME NODE_demo2

int swap_LED(){
    PORTE ^= _BV(PE1);

    return 0;
}


void handle_CAN_msg(uint8_t destID,
                    uint8_t msgID,
                    uint8_t* msg,
                    uint8_t msgLen) {
    

    uint8_t cmd = msg[0];
    //if( cmd & 0x08 ){
    //    swap_LED();
    //}
    swap_LED();
}


ISR(INT1_vect){
    swap_LED();
}


int main(void){
    sei();
    DDRE |= _BV(PE1);

    for(;;){
    }
}
