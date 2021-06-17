
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
  packet.StdId = SENSOR_OUTPUT_CAN_STD_ID; //Identificador estandar con prioridad 2
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
  uint8_t data_input[CAN_MAX_BYTES];

  // Hay que revisar como manejar multiples FIFOS
  uint32_t rx_fill_level;
  if( (rx_fill_level = HAL_CAN_GetRxFifoFillLevel(handle, CAN_RX_FIFO0)) > 0)
  {
    for(int i = 0; i < rx_fill_level; i++)
    {
      if(HAL_CAN_GetRxMessage(handle, CAN_RX_FIFO0, &packet, data[i]) != HAL_OK)
      {
	/* FAILED TO RECEIVE PACKET */
	printf("CAN-RX: Failed to receive packet\n");
	*data[i] = NO_DATA;
      }
      else
      {
	/* PARSE PACKET */
	const int identifier = packet.StdId;
	const int id_type = packet.IDE;
	const int packet_type =  packet.RTR;
	const int bytes = packet.DLC;

	char should_parse = 1;
	if(id_type != CAN_ID_STD)
	{
	  printf("Utilize el identificador CAN estandar, no el extendido\n");
	  should_parse = 0;
	}
	if(packet_type != CAN_RTR_REMOTE)
	{
	  printf("Sensor recibio datos en vez de una petición. Ignorando datos\n");
	  should_parse = 0;
	}
	if(identifier != CONTROL_PANEL_CAN_STD_ID)
	{
	  printf("Sensor recibio paquete de ID que no es del panel de control\n");
	  should_parse = 0;
	}

	if(should_parse)
	{
	  for(int j = 0; j < bytes; j++)
	    data[j] = data_input[j];
	}
	else
	{
	  printf("Not parsing data, deleting it\n");
	  for(int j = 0; j < bytes; j++)
	    data[j] = NO_DATA;    
	}
	
      }

    }
  }

  return handle->ErrorCode;
}
