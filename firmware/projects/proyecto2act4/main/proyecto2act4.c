/*! @mainpage Template
 *
 * @section genDesc General Description
 * configura y utiliza una entrada analógica (CH1) para leer valores de una entrada analógica y lo envía a través de UART para su visualización.
 * y luego se encarga de reproducir una señal ECG almacenada a través de una salida analógica.
 *
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 3/05/2024 | Document creation		                         |
 *
 * @author Doris Micaela Guerrero (doris.guerrero@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "stdint.h"
#include "stdint.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "timer_mcu.h"
#include "uart_mcu.h"
#include "analog_io_mcu.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
#define BUFFER_SIZE 231
#define CONFIG_BLINK_PERIOD_medicion_US 2000
#define CONFIG_BLINK_PERIOD_medicion_US_ecg 4000
/*==================[internal data definition]===============================*/
TaskHandle_t main_task_handle = NULL;
TaskHandle_t ECG_task_handle = NULL;

const char ecg[BUFFER_SIZE] = {
    76, 77, 78, 77, 79, 86, 81, 76, 84, 93, 85, 80,
    89, 95, 89, 85, 93, 98, 94, 88, 98, 105, 96, 91,
    99, 105, 101, 96, 102, 106, 101, 96, 100, 107, 101,
    94, 100, 104, 100, 91, 99, 103, 98, 91, 96, 105, 95,
    88, 95, 100, 94, 85, 93, 99, 92, 84, 91, 96, 87, 80,
    83, 92, 86, 78, 84, 89, 79, 73, 81, 83, 78, 70, 80, 82,
    79, 69, 80, 82, 81, 70, 75, 81, 77, 74, 79, 83, 82, 72,
    80, 87, 79, 76, 85, 95, 87, 81, 88, 93, 88, 84, 87, 94,
    86, 82, 85, 94, 85, 82, 85, 95, 86, 83, 92, 99, 91, 88,
    94, 98, 95, 90, 97, 105, 104, 94, 98, 114, 117, 124, 144,
    180, 210, 236, 253, 227, 171, 99, 49, 34, 29, 43, 69, 89,
    89, 90, 98, 107, 104, 98, 104, 110, 102, 98, 103, 111, 101,
    94, 103, 108, 102, 95, 97, 106, 100, 92, 101, 103, 100, 94, 98,
    103, 96, 90, 98, 103, 97, 90, 99, 104, 95, 90, 99, 104, 100, 93,
    100, 106, 101, 93, 101, 105, 103, 96, 105, 112, 105, 99, 103, 108,
    99, 96, 102, 106, 99, 90, 92, 100, 87, 80, 82, 88, 77, 69, 75, 79,
    74, 67, 71, 78, 72, 67, 73, 81, 77, 71, 75, 84, 79, 77, 77, 76, 76,
};
/*==================[internal functions declaration]=========================*/
/**
 * @brief envia una notificacion a la tarea senial_Task, cuando el timer_medicion_senial alcance su periodo configurado
 */
void FuncTimerA(void* param){
    xTaskNotifyGive(main_task_handle); /* Envía una notificación */
}
/**
 * @brief envia una notificacion a la tarea crearECG_Task, cuando el timer_senial_ECG alcance su periodo configurado
 */
void FuncTimerB(void* param){
    xTaskNotifyGive(ECG_task_handle); /* Envía una notificación*/
}
/**
 * @brief  lee un valor de una entrada analógica y lo envía a través de UART para su visualización.
 */
void senial_Task(void *pvParameter){
uint16_t valor;
    while(1)
    {
     ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* recibe la notificacion */
     AnalogInputReadSingle(CH1, &valor); /* tomo el valor que entra por el canal 1 y lo guardo en valor*/
	 UartSendString(UART_PC,(char*)UartItoa(valor,10)); /*ahora lo mostramos*/
	 UartSendString(UART_PC,"\r\n");

	}
}
/**
 * @brief  se encarga de reproducir una señal ECG almacenada a través de una salida analógica.
 */
void crearECG_Task(void *pvParameter){
uint16_t indice=0;
    while(1)
    {
     ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* recibe la notificacion */
	 AnalogOutputWrite(ecg[indice]); /* escribe en la salida analógica el valor correspondiente al índice actual */
	 indice ++;
	 if(indice == sizeof(ecg)){
		indice= 0;
	 }
	}
}


/*==================[external functions definition]==========================*/
void app_main(void){
	 timer_config_t timer_medicion_senial= {
        .timer = TIMER_A,
        .period = CONFIG_BLINK_PERIOD_medicion_US,
        .func_p = FuncTimerA,
        .param_p = NULL
    };	 
	timer_config_t timer_senial_ECG= {
        .timer = TIMER_B,
        .period = CONFIG_BLINK_PERIOD_medicion_US_ecg,
        .func_p = FuncTimerB,
        .param_p = NULL
    };
	analog_input_config_t senial_ch1={
		.input = CH1,
		.mode = ADC_SINGLE,
		.func_p = NULL,
		.param_p = NULL,
		.sample_frec =0
	};
	analog_input_config_t senial_ch0={
		.input = CH0,
		.mode = ADC_SINGLE,
		.func_p = NULL,
		.param_p = NULL,
		.sample_frec =0
	};
		serial_config_t conf_puerto= {
		.port= UART_PC,
		.baud_rate= 115200,
		.func_p= NULL,
		.param_p= NULL
	};
	TimerInit(&timer_medicion_senial);
	TimerInit(&timer_senial_ECG);

 	AnalogInputInit(&senial_ch1);
	AnalogOutputInit();
	UartInit(&conf_puerto);
	/* Creación de tareas */
	xTaskCreate(&senial_Task, "senial", 2048, NULL, 5, &main_task_handle);
	xTaskCreate(&crearECG_Task, "senial_ecg", 2048, NULL, 5, &ECG_task_handle);
	  /* Inicialización del conteo de los timers */
	TimerStart(TIMER_A);
	TimerStart(TIMER_B);
  
}
/*==================[end of file]============================================*/