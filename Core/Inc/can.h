/**
 * @file	can.h
 * @brief	Header file for can.h
 *
 *  Created on: Apr 8, 2021
 *      Author: Iván Guillermo Peña Flores
 */

#ifndef INC_CAN_H_
#define INC_CAN_H_

#include "stm32f0xx_hal.h"
#include <stdio.h>

/*
Los datos transmitidos comprenderan de un valor de humedad y uno de temperatura
También es posible transmitir la hora, sin embargo eso se le dejara al controlador principal
CAN permite la transmisión de paquetes con información de 0 a 8 bytes, o sea, tenemos dos opciones:
  - Transmitir humedad y temperatura en un mismo paquete, como un float de 4 bytes.
  - Transmitir por separado en dos paquetes, como un double de 8 bytes.
Aqui no es necesaria la precisión de 8 bytes, asi que se optara por un solo paquete.
*/

#define CAN_MAX_BYTES 8 /**> @def Defines max amount of bytes transfered in a CAN packet */

#define NO_DATA 0x00U /**> @def Alias for code readability purposes */

/* POSIBLEMENTE REDUNDANTE
typedef enum can_error {
  CAN_BUFFER_FULL,
  CAN_NO_ACK,
  CAN_OK,
  CAN_TX_OK,
  CAN_RX_OK
} can_error;*/

//Usar banderas de bits
/**
 * @enum Commands that the control panel can emit to sensors
 */
typedef enum can_control {
  CAN_CONTROL_START_TX,
  CAN_CONTROL_STOP_TX,
  CAN_CONTROL_START_READING,
  CAN_CONTROL_STOP_READING
} can_error;

typedef CAN_TxHeaderTypeDef can_tx_packet; /**> @typedef Alias for CAN_TxHeaderTypeDef */
typedef CAN_RxHeaderTypeDef can_rx_packet; /**> @typedef Alias for CAN_RxHeaderTypeDef */
typedef CAN_HandleTypeDef can_handle; /**> @brief Alias for CAN_HandleTypeDef */

/* Almacena mailboxes activos */
static uint32_t tx_mailboxes = 0;
//static uint32_t rx_fifo = 0;

uint32_t can_write_to_mailbox(can_handle* handle, uint8_t* data, int bytes);
uint32_t can_get_from_fifo(can_handle* handle, uint8_t* data[]);

#endif /* INC_CAN_H_ */
