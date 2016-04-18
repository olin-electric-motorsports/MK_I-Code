#ifndef CAN_API_H
#define CAN_API_H

#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdlib.h>

/* Node name definitions */
//#define send_demo          ((uint8_t) 0x0)
//#define receive_demo          ((uint8_t) 0x1)

/* Message IDs */
#define IDT_demo         ((uint8_t) 0x00)
#define IDT_watchdog     ((uint8_t) 0x01)
#define IDT_BMS_err      ((uint8_t) 0x02)
#define IDT_BMS          ((uint8_t) 0x03)
#define IDT_throttle     ((uint8_t) 0x04)

/* Message Lengths */
#define IDT_demo_l       ((uint8_t) 0x01)
#define IDT_watchdog_l   ((uint8_t) 0x01)
#define IDT_BMS_err_l    ((uint8_t) 0x01)
#define IDT_BMS_l        ((uint8_t) 0x01)
#define IDT_throttle_l   ((uint8_t) 0x01)

/* Masks */
#define IDM_global       ((uint8_t) 0x00)
#define IDM_single       ((uint8_t) 0xff)

/* Error messages */
#define ERR_UNDERVOLT        ((uint8_t) 0x00)

/* Function Prototypes */
uint8_t CAN_init( uint8_t interrupt_depth );

uint8_t CAN_Tx  ( uint8_t mob, uint8_t ident, uint8_t msg_length, uint8_t msg[]);

uint8_t CAN_Rx  ( uint8_t mob, uint8_t ident, uint8_t msg_length, uint8_t mask);

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

#endif
