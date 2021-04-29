# Composteador-DICyT
Software para el composteador desarrollado en la DICyT.

HASTA QUE SEA POSIBLE PROTOTIPAR, ESTE REPOSITORIO SE MANTENDRA ASI
SE ENFOCARA EN EL PANEL DE CONTROL PRINCIPAL POR EL MOMENTO
REVISE EL TODO

### Archivos fuente

```
- ./Core/Src/
  - main.c
  - can.c
  - sensors.c
- ./Core/Inc/
  - main.h
  - can.h
  - sensors.h
``` 

### Compilación

Se requeriran los siguientes defines en la compilación:

```
USE_HAL_TIM_REGISTER_CALLBACKS 1 /* Para el uso de callbacks definidos por el usuario y no los por defecto. */
SENSOR_OUTPUT_CAN_STD_ID 0xXX /* Para identificar el sensor especifico del que se manda el dato */
OTHER_SENSOR_CAN_STD_ID 0xXX /* Para identificar datos del otro sensor, que seran ignorados a favor de la señal del panel de control principal */
CONTROL_PANEL_CAN_STD_ID 0xXX /* Para identificar mensajes del panel de control principal */
```
Por el momento, el identificador del otro sensor es redundante.

### TODO
- Implementar ciclo principal del programa.
- Implementar interrupt adecuado para el timer, falta prototipado para verificar el funcionamiento.
- Hacer uso de multiples FIFOs al recibir datos (¿Es necesario en primer lugar?)
- Implementar función Error_Handler() adecuadamente.
- Usar banderas de bits al reportar errores, o comandos de control


### Integrantes

- Iván Guillermo Peña Flores (mudzap)
- Luis Ignacio Velásquez Casado 
- José Omar Sánchez Coppola (apasin-josc)
