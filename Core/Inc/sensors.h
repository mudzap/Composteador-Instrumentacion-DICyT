/** @file	sensors.h
 *  @brief	Header file for sensors.c
 *
 *  Created on: Apr 10, 2021
 *      Author: Iván Guillermo Peña Flores
 */

#ifndef INC_SENSORS_H_
#define INC_SENSORS_H_

#include "stm32f0xx_hal.h"
#include <stdio.h>

#define CLOCK_RATE 48000 /* Frecuencia del timer en kHz*/
#define FREQ_LUT_SIZE 21 /* Numero de elementos de LUT */
#define FREQ_LUT_INTERVAL 5 /* Intervalo de LUT en %RH */

#define MAX_TIMER_SAMPLES 3 /* Muestreos del timer para adquirir frecuencia */

#define ADC_TIMEOUT 100 /* Tiempo maximo para realizar un muestro */

typedef enum sensor_error {
  ALL_OK = 0,
  TEMP_SENSOR_FAIL = 1,
  HUM_SENSOR_FAIL = 2
} sensor_error;

typedef enum temp_error {
  TEMP_OK = 0,
  TEMP_INTERNALSENSOR_FAIL = 1,
  TEMP_ADC_FAIL = 2
} temp_error;

typedef enum hum_error {
  HUM_OK = 0,
  HUM_TOO_LARGE = 1,
  HUM_TOO_SMALL = 2,
  HUM_TIM2_FAIL = 4
} hum_error;


/*
 * para manejar los handles fuera del contexto de main, los almacenamos en un struct para
 * su facil manejo
 */
typedef ADC_HandleTypeDef adc_handle;
typedef TIM_HandleTypeDef tim_handle;
typedef struct sensors_handle {
  adc_handle adc;
  tim_handle htim2;
} sensors_handle;

/* Los valores negativos indican un estado de error, estos no se especifican en la hoja de datos, se asume
 * que porque no son factibles en la practica. Como se menciono en la documentación, esta LUT se da en intervalos
 * de 5 en 5 de %RH, desde el 0 al 100.
 */
static const float freq_lut[FREQ_LUT_SIZE] = {
    -1.0, -1.0, 7155, 7080, 7010, 6945,
    6880, 6820, 6760, 6705, 6650, 6600,
    6550, 6500, 6450, 6400, 6355, 6305,
    6260, 6210, -1.0
};

static uint32_t timer_samples[MAX_TIMER_SAMPLES];

static float rel_humidity = -1.f;
static float temperature = -1.f;

sensor_error read_sensors(sensors_handle* handle, float* temp, float* rh);
temp_error read_temp(adc_handle* handle, float* temp);
temp_error read_temp_adc(adc_handle* handle, float* temp);
temp_error read_temp_internal(float* temp);

hum_error read_rh(tim_handle* handle, float* rh);
void init_tim_callback(tim_handle* handle);
void recursive_tim_callback(tim_handle* handle, int sample);

#endif
