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
#include "stm32f0xx_it.h"

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
  
  temp_error temp_read_error = read_temp(&handle->adc, temp);
  hum_error rh_read_error = read_rh(&handle->htim2, rh);

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

#ifdef USE_INTERNAL_TEMP_SENSOR_AS_FALLBACKs
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
  if(handle->State != HAL_OK)
  {
    printf("Timer still getting freq values\n");
    return HUM_TIM2_FAIL;
  }
  else
  {
    float avg_freq = 0.f;

    // t_real = timer_val * 1/clock_rate, ergo:
    // f_real = clock_rate / timer_val
    for(int i = 1; i <= MAX_TIMER_SAMPLES; i++)
    {
      uint32_t time_diff;
      uint32_t time_n = timer_samples[i];
      uint32_t time_n_m1 = timer_samples[i-1];
      
      time_diff = (time_n > time_n_m1) ?     /* Como manejar overflows en los timers? */
	time_n - time_n_m1 :                 /* t[n] - t[n-1] */
	time_n + (0xFFFFFFFF - time_n_m1);   /* t[n] + ((2^32-1) - t[n-1]) */

      avg_freq += TIMER_CLOCK_RATE/time_diff;
    }
    avg_freq /= MAX_TIMER_SAMPLES;
    
    *rh = lerp_rh_from_lut(avg_freq);

    // Es posible hacer uso de DMA para inyectar directamente el valor del timer
    // Revisar si handle->Channel si da los valores TIM_CHANNEL_N correctos para
    // la función
    
    // Establece callback
    // Hay que definir USE_HAL_TIM_REGISTER_CALLBACKS a 1 en el compilador
    // para que funcione los callbacks de usuario.
    TIM_ITRx_SetConfig(handle->Instance, TIM_TS_ETRF); /* Use external trigger input */

    //FILTRO ES UN PLACEHOLDER, VER INTERRUPTS
    TIM_ETR_SetConfig(handle->Instance, TIM_ETRPRESCALER_DIV1, TIM_ETRPOLARITY_NONINVERTED, 0x00); /* Conf ext trig*/
    HAL_TIM_RegisterCallback(handle, HAL_TIM_TRIGGER_CB_ID, init_tim_callback);

  }

  return HUM_OK;

}

/*  Ver documentación 20 de abril, 2021
 *  Lectura_Humedad.pdf
 */
static int callback_iteration;
void init_tim_callback(tim_handle* handle)
{
  //Starts timer
  HAL_TIM_RegisterCallback(
    handle, HAL_TIM_TRIGGER_CB_ID,
    recursive_tim_callback);
  callback_iteration = 0;
  HAL_TIM_Base_Start_IT(handle);
  
}

void recursive_tim_callback(tim_handle* handle)
{
  callback_iteration++;
  timer_samples[callback_iteration] = TIM_GetCounter(handle->Instance);

  callback_iteration++;
  if(callback_iteration >= MAX_TIMER_SAMPLES)
  {
    HAL_TIM_UnRegisterCallback(handle, HAL_TIM_TRIGGER_CB_ID);
  }
}

/**
 * @brief	Obtains and returns the RH% by means of interpolation by using data
 * 		from the LUT.
 * @param	float: Freq data
 * @retval	float: RH value
 */
float lerp_rh_from_lut(float freq)
{

  // Encuentra los valores por los que esta rodeado el valor de frec en la LUT
  // Si no cabe, da un valor extremo (0, o 100)
  // La complejidad O(N) no es problema, pienso yo
  // Seria interesante almacenar la LUT en otras estructuras de datos
  // como un arbol binario, pero no pienso esto sea gran problema
  int il = 0;
  int ig = 0;
  if(freq < freq_lut[0]) {
    return 0.f;
  }
  else if (freq > freq_lut[FREQ_LUT_SIZE-1]) {
    return 100.f;
  }
  else {
    while(freq < freq_lut[ig]) {
      ig++;
    }
  }

  il = ig - 1;

  double new_rh = 0;
  new_rh = ( (freq - freq_lut[il]) * (ig*5.f - il*5.f) ) / (freq_lut[ig] - freq_lut[il]) + il*5.f;
  return new_rh;
}
