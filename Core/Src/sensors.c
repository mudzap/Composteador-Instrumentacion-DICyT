/**
 *  @file 	sensors.c
 *  @brief	Abstraction layer for sensor readings
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
 * EVITAR HACIENDO DEREFERENCIAS CONSTANTES, O DE OTRA FORMA, ALMACENARLO EN
 * VARIABLES ESTATICAS GLOBALES
 */

/**
 * @brief	Reads data from both the humidity and temperature sensors.
 *              The humidity sensor should have priority, since it is very
 *              dependant on the timer, and so, will be managed by means of
 *              an interrupt or callback function.
 * @param	pointer to sensors_handle: Handle struct containing the
 *              handle to both the timer and adc components.
 * @param       pointer to float storing temperature (might use static global)
 * @param       pointer to float storing rh (ditto)
 *
 * @retval	Sensor read error flags
 */
sensor_error read_sensors(sensors_handle* handle, float* temp, float* rh)
{
  
  temp_error temp_read_error = read_temp(&handle->adc);
  hum_error rh_read_error = read_rh(&handle->htim2);

  sensor_error sensor_error_flags = ALL_OK;
  if(temp_read_error != TEMP_OK) {
    sensor_error_flags |= TEMP_SENSOR_FAIL;  
  }
  if(rh_read_error != HUM_OK) {
    sensor_error_flags |= HUM_SENSOR_FAIL;
  }

  return sensor_error_flags;
}

/**
 * @brief	Reads data from the temperature sensor, handles any possible errors
 * 		and falls back to the internal temperature sensor in case the ADC fails.
 * @param	adc_handle*: Pointer to ADC handle object
 * @param	float*: Pointer to float to store temperature
 *
 * @retval	Sensor error
 */
temp_error read_temp(adc_handle* handle, float* temp)
{
  temp_error error_flags = TEMP_OK;
  
  if(read_temp_adc(handle, temp) == TEMP_ADC_FAIL)
  {

#ifdef USE_INTERNAL_TEMP_SENSOR_AS_FALLBACK
    if(get_temp_internal(temp) == TEMP_INTERNALSENSOR_FAIL)
    {
      error_flags |=  TEMP_INTERNALSENSOR_FAIL;
    }
#endif
    
    error_flags |= TEMP_ADC_FAIL;
    
  }
  return error_flags; 
}

/**
 * @brief	Reads data from the temperature sensor utilizing an ADC
 * @param	adc_handle*: Pointer to ADC handle object
 * @param	float*: Pointer to float to store temperature
 *
 * @retval	Sensor error
 */
temp_error read_temp_adc(adc_handle* handle, float* temp)
{
  if (HAL_ADC_Start(handle) == HAL_BUSY)
  {
    printf("ADC busy, can't read.\n");
    return TEMP_ADC_FAIL;
  }
  else
  {
    // Es posible leer de ambos ADCs al mismo tiempo, usando dos canales
    // considerar si esto es necesario. Lo sera solamente si el tiempo de
    // lectura del ADC empieza a afectar negativamente la lectura del bus
    // CAN (La lectura de humedad no es problema, ya que para esta se
    // utilizaran interrupts y callbacks).

    //READ V_REF ADC
    int v_ref_read;
    if(HAL_ADC_PollForConversion(handle, ADC_TIMEOUT) == HAL_OK)
    {
      v_ref_read = HAL_ADC_GetValue(handle);
    }
    else
    {
      printf("ADC Timed out reading Vref\n");
      return TEMP_ADC_FAIL;
    }

    //READ LM35 ADC
    int temp_reading;
    if(HAL_ADC_PollForConversion(handle, ADC_TIMEOUT) == HAL_OK)
    {
      temp_reading = HAL_ADC_GetValue(handle);
    }
    else
    {
      printf("ADC Timed out reading external temp sensor\n");
      return TEMP_ADC_FAIL;
    }

    //Calcular temperatura en grados centigrados
    //Referirse a la documentación 'Software_Instrumentacion.pdf'
    //del 7 de abril de 2021
    //constexpr se especifica en el estandar C++11
    const float v_supply_sqr = 3.3*3.3;
    const float v_ref = 1.25;
    const float v_temp_grad = 0.01;
    const float resolution = 4096; //2^12

    //temp = V / V/°C = (v_uncal * read_value / 4096) / 10 mV/°C 
    //V_read_ref = 1.25 = V_uncalibrated * read_value / 4096
    // por lo tanto, V_uncalibrated = 1.25 * 4096 / read_value
    float v_uncal = (v_ref * resolution) / v_ref_read;
    float temp_deg_c = (v_uncal * temp_reading / resolution) / 0.01;
    *temp = temp_deg_c;
    
    return TEMP_OK;
  }
}

#ifdef USE_INTERNAL_TEMP_SENSOR_AS_FALLBACK
/**
 * @brief	Reads data from the internal temperature sensor,
 * @param	float*: Pointer to float to store temperature
 *
 * @retval	Sensor error
 */
temp_error read_temp_internal(float* temp)
{
	return TEMP_OK;
}
#endif


/**
 * @brief     Reads RH by translating it from the external oscillating
 *            frequency of the external RC oscillator.
 * @param     timer handle*: Pointer to timer handle which will
 * @param     float*: Pointer to float to store RH
 *
 * @retval    hum sensor error
 */
hum_error read_rh(tim_handle* handle, float* rh)
{

  // Es posible hacer uso de DMA para inyectar directamente el valor del timer
  // Revisar si handle->Channel si da los valores TIM_CHANNEL_N correctos para
  // la función
  if(HAL_TIM_IC_Start(handle, handle->Channel) != HAL_OK)
    {
      return HUM_TIM2_FAIL;
      printf("Timer busy, not reading RH until second callback is called\n");
    }
  else
    {
      //Establece callback
      
    }

}

/*  Ver documentación 20 de abril, 2021
 *  Lectura_Humedad.pdf
 */
void init_tim_callback(tim_handle* handle)
{
  
}

void recursive_tim_callback(tim_handle* handle, int sample)
{
  
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
