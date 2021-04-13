/*
 * @file 	sensors.c
 * @brief	Abstraction layer for sensor readings
 *
 *  Created on: Apr 10, 2021
 *      Author: Iván Guillermo Peña Flores
 */

/*
 * Este programa es una traducción del programa original, con los cambios siendo que la LUT ya no
 * almacena los valores de capacitancia, sino de frequencia.
 * Los cambios necesarios a adaptar seran:
 *  - ADC
 *  - TIMERS
 *  - CLOCK PRESCALING
 */

#include "sensors.h"

/* ESTOY CONSIDERANDO CAMBIAR QUE RETORNEN POR COPIA, NO POR REFERENCIA, PARA ASI
 * EVITAR HACIENDO DEREFERENCIAS CONSTANTES
 */

/**
 * @brief	Reads data from both the humidity and temperature sensors.
 * @param	MAYBE? STILL NOT IMPLEMENTED, SHOULD BE HANDLE OBJETS
 *
 * @retval	Sensor error
 */
int read_sensors()
{
	temperature = get_temp();
	rel_humidity = get_rh();
}

/**
 * @brief	Reads data from the temperature sensor, handles any possible errors
 * 			and falls back to the internal temperature sensor in case the ADC fails.
 * @param	adc_handle*: Pointer to ADC handle object
 * @param	float*: Pointer to float to store temperature
 *
 * @retval	Sensor error
 */
int read_temp(adc_handle* handle, float* temp)
{

	if(get_temp_adc(handle, temp) == TEMP_ADC_FAIL)
	{

		if(get_temp_internal(temp) == TEMP_INTERNALSENSOR_FAIL)
		{
			return TEMP_INTERNALSENSOR_FAIL;
		}

		return TEMP_ADC_FAIL;
	}

	return TEMP_OK;
}

/**
 * @brief	Reads data from the temperature sensor utilizing an ADC
 * @param	adc_handle*: Pointer to ADC handle object
 * @param	float*: Pointer to float to store temperature
 *
 * @retval	Sensor error
 */
int read_temp_adc(adc_handle* handle, float* temp)
{
	//READ V_REF ADC

	//READ LM35 ADC
	//10 mV/°C
	//12 bits
	//VER DOCUMENTACIÓN, AHI ESTA EL CALCULO
	*temp = 0.1;

	*temp = 1;

	return TEMP_OK;
}

/**
 * @brief	Reads data from the internal temperature sensor,
 * @param	float*: Pointer to float to store temperature
 *
 * @retval	Sensor error
 */
int read_temp_internal(float* temp)
{
	return TEMP_OK;
}

/*
 *
 * 	TODO DE AQUI EN ADELANTE NO HA SIDO TRADUCIDO
 * 	PERO SI PARCIALMENTE DOCUMENTADO
 *
 */


//ISR( ANA_COMP )
//Alternativa superior, pero pierde compatibilidad con PLC
//Si surge el interes a que quiero decir, revisar la datasheet del
//ATtiny 84, sección 15: 'Analog Comparator'
/*
ISR( INT0_vect )
{
  //WOW
  //if (conditionChecker.checkConditional(cond.tostr() == 'true' && cond.tostr() != 'false'))
  //no hagan esto pls
  trigger();
}

void trigger() {
  //STOP TIMER
  GTCCR |= (1 << TSM) | (1 << PSR10);

  //DISABLE INTERRUPTS
  cli();

  uint16_t timerValue = 0;
  timerValue |= (TCNT1L | (TCNT1H << 8));
  //AJUSTA VALOR DE TIMER
  timerValue += 4;

  discharge();

  //QUIZA SEAN SUFICIENTES OPERACIONES DE PUNTO FLOTANTE
  //PARA DAR TIEMPO PARA QUE SE DESCARGE EL SENSOR
  //IGUAL EL TIEMPO DE CONVERSION DEL ADC PARA EL SENSOR
  //DE TEMPERATURA, DEBERIA DE DAR TIEMPO SUFICIENTE
  //PROBAR CON OSCILOSCOPIO DE TODAS FORMAS
  RH = interpRH(timerValue);
}


// DUH
void getTemp() {
  //READ ADC
  ADCSRA |= (1 << ADSC);
  uint16_t reading = 0;

  //WAIT FOR READING
  //DON'T USE ADC INTERRUPT AS TO GIVE TIMER PRIORITY
  while (ADCSRA & (1<<ADSC)) {}

  //GET ADC
  reading |= (ADCL | (ADCH << 8));

  //TRANSLATE TO VOLTAGE
  temp = (REF_V * reading)/1024.f;
}
*/

/**
 * @brief	Obtains and returns the RH% by means of interpolation by using data
 * 			from the LUT.
 * @param	uint32_t: Timer data
 * @param	float*: Pointer to float to store RH%
 *
 * @retval	Sensor error
 */
/*
int interpRH(uint32_t timerValue, float* rh) {
  double realTime = timerValue*CLOCK_RATE;
  double charge = realTime*CHARGE_CURRENT;
  double cap = charge/(SUPPLY_V - REF_V);

  //FIND WHICH C VALUES IS IT SURROUNDED WITH IN THE LUT
  //IF IT DOESNT FIT LUT, GIVE MIN OR MAXIMUM RH VALUE
  //O(N) COMPLEXITY? NO PROBLEM
  int il = 0;
  int ig = 0;
  if(cap < LUT[0]) {
    return 0.f;
  }
  else if (cap > LUT[LUT_SIZE-1]) {
    return 100.f;
  }
  else {
    while(cap < LUT[ig]) {
      ig++;
    }
  }

  il = ig - 1;

  double new_rh = 0;
  new_rh = ( (cap - LUT[il]) * (ig*5.f - il*5.f) ) / (LUT[ig] - LUT[il]) + il*5.f;
  *rh = new_rh;
}

*/
