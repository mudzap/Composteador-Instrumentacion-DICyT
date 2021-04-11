/* main_loop.h
 *
 *
 *  Created on: Apr 8, 2021
 *      Author: Iván Guillermo Peña Flore
 */

typedef enum temp_error {
	TEMP_INTERNALSENSOR_FAIL,
	TEMP_ADC_FAIL,
	TEMP_OK
};

typedef enum hum_error {
	HUM_TOO_LARGE,
	HUM_TOO_SMALL,
	HUM_TIM2_FAIL,
	HUM_OK
} hum_error;





/*
 * para manejar los handles fuera del contexto de main, los almacenamos en unos structs para
 * su facil manejo
 */
struct sensors_h {
	ADC_HandleTypeDef adc;
	TIM_HandleTypeDef htim2;
};

/* Los valores negativos indican un estado de error, estos no se especifican en la hoja de datos, se asume
 * que porque no son factibles en la practica. Como se menciono en la documentación, esta LUT se da en intervalos
 * de 5 en 5 de %RH, desde el 0 al 100.
 */
float freq_lut[FREQ_LUT_SIZE] = {
		-1.0, -1.0, 7155, 7080, 7010, 6945,
		6880, 6820, 6760, 6705, 6650, 6600,
		6550, 6500, 6450, 6400, 6355, 6305,
		6260, 6210, -1.0
};

float rel_humidity = -1.f;
float temperature = -1.f;


float temp_read_lm35()
{

}

float temp_read_internal()
{

}

float hum_read_hs1101()
{

}

#endif /* INC_MAIN_LOOP_H_ */
