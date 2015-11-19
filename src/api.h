#ifndef CAN_API_H
#define CAN_API_H

#define F_CPU (1000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define NODE_demo1          ((uint8_t) 0x0)
#define NODE_demo2          ((uint8_t) 0x1)

#define MSG_demoMsg         ((uint8_t) 0xA)

int CAN_init(uint8_t nodeID);

void read_msg(void);

void setup_mobs(uint8_t nodeID);

int send_CAN_msg(uint8_t destID, uint8_t msgID, uint8_t msg[], uint8_t msgLength);

void handle_CAN_msg(uint8_t destID, uint8_t msgID, uint8_t msg[], uint8_t msgLen);

#endif
