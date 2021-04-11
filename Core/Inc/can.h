/*
 * can.h
 *
 *  Created on: Apr 8, 2021
 *      Author: Iván Guillermo Peña Flores
 */

#ifndef INC_CAN_H_
#define INC_CAN_H_

#include "stm32f0xx_hal.h"

typedef enum can_error {
  CAN_BUFFER_FULL,
  CAN_NO_ACK,
  CAN_OK,
  CAN_TX_OK,
  CAN_RX_OK
} can_error;

#define CAN_TX_TIMEOUT 10

/*
Los datos transmitidos comprenderan de un valor de humedad y uno de temperatura
También es posible transmitir la hora, sin embargo eso se le dejara al controlador principal
CAN permite la transmisión de paquetes con información de 0 a 8 bytes, o sea, tenemos dos opciones:
  - Transmitir humedad y temperatura en un mismo paquete, como un float de 4 bytes.
  - Transmitir por separado en dos paquetes, como un double de 8 bytes.
Aqui no es necesaria la precisión de 8 bytes, asi que se optara por un solo paquete.
*/

typedef can_tx_packet CAN_TxHeaderTypeDef;
typedef can_rx_packet CAN_RxHeaderTypeDef;

/* Para el bus can, solamente basta con typedef */
typedef can_handle CAN_HandleTypeDef;


int can_write_to_fifo(can_handle* handle, void* data, char bytes);
can_packet can_get_from_fifo();

#endif /* INC_CAN_H_ */
