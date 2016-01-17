#ifndef CAN_API_H
#define CAN_API_H

#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdlib.h>

/* Node name definitions */
#define send_demo          ((uint8_t) 0x0)
#define receive_demo          ((uint8_t) 0x1)

/* Message IDs */
#define MSG_demo         ((uint8_t) 0xA)

/* Function Prototypes */
int CAN_init( void );

int CAN_Tx(uint8_t nodeID, uint8_t msg[], uint8_t msg_length);

int CAN_Rx( uint8_t nodeID, uint8_t msg_length);

/* Notes on Usage:
 *      CAN_init MUST be called first. It sets up a variety of settings
 *      for using the CAN functionality.
 *
 *      A CAN_mob must be set as SEND in order to send a message.
 *
 *      To receive messages, you must use the CAN interrupt:
 *              ISR(CAN_INT_vect)
 *      in your code.
 */

//void CAN_read_msg(void);


//int CAN_set_MOb_RX(uint8_t nodeID);
//int CAN_set_MOb_TX(uint8_t nodeID);

//void CAN_handle_msg(uint8_t destID, uint8_t msgID, uint8_t msg[], uint8_t msgLen);

#endif
