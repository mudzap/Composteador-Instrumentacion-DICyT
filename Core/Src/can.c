/**
 * @file 	can.c
 * @brief	Abstracts read and write functionality of the CAN bus
 *
 *  Created on: Apr 8, 2021
 *      Author: Iván Guillermo Peña Flores
 */

#include "can.h"

/**
 * @brief	Writes the content from a data buffer into the mailbox, to be sent in the CAN bus
 * @param	can_handle*: Pointer to a handle to a CAN object, typedefs CAN_HandleTypeDef
 * @param	uint8_t*: Pointer to the data buffer
 * @param	int: Number of bytes to write, can't be larger than CAN_MAX_BYTES
 *
 * @retval	CAN error
 */
uint32_t can_write_to_mailbox(can_handle* handle, uint8_t* data, int bytes)
{
  can_tx_packet packet;
  packet.StdId = 0x02; //Identificador estandar con prioridad 2
  packet.IDE = CAN_ID_STD; //Identificador estandar
  packet.RTR = CAN_RTR_DATA; //Tipo de paquete (Data frame / Remote frame)
  packet.DLC = bytes; //Tamaño del paquete

  if(bytes > CAN_MAX_BYTES) {
	  printf("CAN-TX: Se intentaron enviar %d bytes, el maximo es %d.\n", bytes, CAN_MAX_BYTES);
	  return handle->ErrorCode;
  }

  if(HAL_CAN_GetTxMailboxesFreeLevel(handle) > 0)
  {
	  if(HAL_CAN_AddTxMessage(handle, &packet, data, &tx_mailboxes) != HAL_CAN_ERROR_NONE)
	  {
		  Error_Handler();
	  }
  }
  else
  {
	  //MAILBOXES LLENOS
	  printf("CAN-TX: Mailboxes llenos, no enviando los siguientes datos: 0x");
	  for(int i = 0; i < packet.DLC; i++)
	  {
	     printf("%X", *(data + i) );
	  }
	  printf("\n");
  }

  return handle->ErrorCode;
}

/**
 * @brief	Reads the content from a CAN FIFO buffer into a given data buffer
 * @param	can_handle*: Pointer to a handle to a CAN object, typedefs CAN_HandleTypeDef
 * @param	uint8_t*: Pointer to the data buffer
 *
 * @retval	CAN error
 */
uint32_t can_get_from_fifo(can_handle* handle, uint8_t* data[])
{
	can_rx_packet packet;

	uint32_t rx_fill_level;
	if( (rx_fill_level = HAL_CAN_GetRxFifoFillLevel(handle, rx_fifo)) > 0)
	{
		for(int i = 0; i < rx_fill_level; i++)
		{
			if(HAL_CAN_GetRxMessage(handle, rx_fifo, &packet, data[i]) != HAL_OK)
			{
				/* FAILED TO RECEIVE PACKET */
				printf("CAN-RX: Failed to receive packet\n");
				*data[i] = NO_DATA;
			}
			else
			{
				/* PARSE PACKET */
				// Data already on array
				//packet->DLC
				//packet->...
			}

		}
	}

	return handle->ErrorCode;
}
