/*
 * can.c
 *
 *  Created on: Apr 8, 2021
 *      Author: Iván Guillermo Peña Flores
 */

uint32_t can_write_to_fifo(can_handle* handle, void* data, char bytes)
{
  can_tx_packet packet;
  packet.StdId = 0x02; //Identificador estandar con prioridad 2
  packet.IDE = CAN_ID_STD; //Identificador estandar
  packet.RTR = CAN_RTR_DATA; //Tipo de paquete (Data frame / Remote frame)
  packet.DLC = bytes; //Tamaño del paquete

  if(HAL_CAN_Transmit(packet, TIMEOUT) == HAL_CAN_ERROR_NONE)
  {
	  Error_Handler();
  }

  return handle.ErrorCode;
}

uint32_t can_get_from_fifo()
{
	can_rx_packet packet = HAL_CAN_Receive();


}
