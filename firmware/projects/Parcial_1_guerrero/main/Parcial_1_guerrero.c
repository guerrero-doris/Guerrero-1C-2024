/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * Diseñar un dispositivo basado en la ESP-EDU que permita controlar el
 * riego y el pH de una plantera. El sistema está compuesto por una serie de recipientes 
 * con agua, una solución de pH ácida y otra básica, un sensor de húmedad y uno de pH, y tres bombas peristálticas para los líquidos. 
 * se debe controlar el suministro de agua. El sensor de humedad se conectaa un GPIO de la placa y cambia su estado de “0” a “1” lógico cuando la humedad
 * presente en la tierra es inferior a la necesaria. La bomba de agua deberá encenderse
 * en dicho caso.
 * El pH de la plantera se debe mantener entre el rango de 6 a 6,7. Se cuenta con un 
 * sensor de pH analógico que brinda una salida de 0 a 3V para el rango de pH de 0 a 14. 
 * Por debajo de 6 se debe encender la bomba de la solución básica, por encima de 6.7
 * se debe encender la bomba de la solución ácida.La medición de agua y pH se debe realizar cada 3 segundos. Las bombas se
 * encienden colocando en alto un GPIO conectado a las mismas.
 * Se informará el estado del sistema a través de la UART mediante mensajes cada 5
 * segundos
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection

 * 
 * |   SENSOR			|   ESP32		    |
 * |:------------------:|:------------------|
 * | 	VCC				| 	+3.3V			|
 * | 	CH1          	|    ANALOG OUTPUT  |
 * | 	CH2				|    ANALOG OUTPUT  |
 * |    GND			    |	GND				|
 * 
 * |   Device 1		|   ESP32    	|
 * |:--------------:|:--------------|
 * | 	bomba_1	 	| 	GPIO_1   	|
 * | 	bomba_2	 	| 	GPIO_2		|
 * |    bomba_3     |   GPIO_3      |
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 11/06/2024 | Document creation		                         |
 * C
 *
 * @author Guerrero, Doris Micaela
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "gpio_mcu.h"
#include "analog_io_mcu.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "timer_mcu.h"
#include "uart_mcu.h"
#include "switch.h"
/*==================[macros and definitions]=================================*/
#define V_REF 3.3								// valor de tension de referencia
#define TOTAL_BITS 1024							// cantidad de bits
#define CONFIG_BLINK_PERIOD_medicion_US 3000000 // PORQUE DEBE MEDIR CADA 3 SEG
#define CONFIG_BLINK_PERIOD_medicion_US_ESTADO 5000000 //debe infomar el estado cada 5 seg
/*==================[internal data definition]===============================*/
bool encender_bomba_agua = false;
bool encender_SolBasica = false;
bool encender_SolAcida = false;
float valor_ph;
float ph;
float valor_humedad;
float conversion_humedad;
float valor_humedad_umbral = 3.3; // si marca 100 esta 100% humedo
bool start = false;
bool hold = true;
TaskHandle_t medicion_task_handle = NULL;
TaskHandle_t Informar_task_handle = NULL;

/*==================[internal functions declaration]=========================*/

// sensor de humedad
// sensor de ph-> este activa dos bombas (solAcida y solBasica)
/**
 * @brief envia una notificacion a la tarea senial_medir_task, cuando el timer alcance su periodo configurado
 */
void FuncTimerA(void *param)
{
	xTaskNotifyGive(medicion_task_handle); /* Envía una notificación */
}
/**
 * @brief envia una notificacion a la tarea Informar_estado_Task, cuando el timer alcance su periodo configurado
 */
void FuncTimerB(void *param)
{
	xTaskNotifyGive(Informar_task_handle); /* Envía una notificación*/
}
/**
 * @brief  lee un valor de una entrada analógica y la convierte a voltios.
 */
void senial_medir_task(void *pvParameter)
{
	while (1)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (start)
		{
			// obtengo el valor del ph -> con esto veo si es menor o mayor y activo su respectiva bomba
			AnalogInputReadSingle(CH1, &valor_ph);
			float voltage = (valor_ph / TOTAL_BITS) * 3.0; // Convertir a voltaje (0-3V)
			ph = (voltage / V_REF) * 14.0;				   // Convertir a pH (0-14)
			// obtengo el valor de la humedad
			AnalogInputReadSingle(CH2, &valor_humedad);
			conversion_humedad = valor_humedad * (V_REF / TOTAL_BITS); // valor medido a voltios
		}
	}
}
/**
 * @brief  toma los valores, los compara y enciende o apaga las bombas. Tambien lo envía informacion sobre esto a través de UART para su visualización.
 */
void Informar_estado_task(void *pvParameter)
{
	while (1)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (hold)
		{
			if (ph < 6.0)
			{
				encender_SolBasica = true;
				GPIOOn(GPIO_3);
				UartSendString(UART_PC, (char *)UartItoa(valor_ph, 10));
				UartSendString(UART_PC, "El ph esta acido \r\n");
				UartSendString(UART_PC, (char *)UartItoa(valor_ph, 10));
				UartSendString(UART_PC, "Enciendo bomba de solucion basica \r\n");
			}
			else if (ph > 6.7)
			{
				encender_SolAcida = true;
				GPIOOn(GPIO_2);
				UartSendString(UART_PC, (char *)UartItoa(valor_ph, 10));
				UartSendString(UART_PC, "El ph esta basico \r\n");
				UartSendString(UART_PC, (char *)UartItoa(valor_ph, 10));
				UartSendString(UART_PC, "Enciendo bomba de solucion acida \r\n");
			}
			else
			{
				encender_SolAcida = false;
				encender_SolBasica = false;
				GPIOOff(GPIO_2);
				GPIOOff(GPIO_3);
				UartSendString(UART_PC, (char *)UartItoa(ph, 10));
				UartSendString(UART_PC, "El ph esta optimo \r\n");
			}

			if (conversion_humedad  < valor_humedad_umbral)
			{
				encender_bomba_agua = true;
				GPIOOn(GPIO_1);
				UartSendString(UART_PC, (char *)UartItoa(ph, 10));
				UartSendString(UART_PC, "Bomba de agua encendida \r\n");
			}
			else
			{
				encender_bomba_agua = false;
				GPIOOff(GPIO_1);
				UartSendString(UART_PC, (char *)UartItoa(ph, 10));
				UartSendString(UART_PC, "Bomba de agua apagada\r\n");
			}
		}
	}
}
/**
 * @brief cambia el valor de la variable global
 */
void encender_programa()
{
	start = !start;
}
/**
 * @brief cambia el valor de la variable global
 */
void parar_programa()
{
	hold = !hold;
}
/*==================[external functions definition]==========================*/
void app_main(void)
{								   // gpio para el control de las bombas
	GPIOInit(GPIO_1, GPIO_OUTPUT); // bomba_de_agua ->BOMBA 1
	GPIOInit(GPIO_2, GPIO_OUTPUT); // bomba_de_solAcida ->BOMBA 2
	GPIOInit(GPIO_3, GPIO_OUTPUT); // bomba_de_solBasica ->BOMBA 3
	// entradas analogicas de los sensores de humedad y ph
	analog_input_config_t senial_PH_sensor = {
		.input = CH1,
		.mode = ADC_SINGLE,
		.func_p = NULL,
		.param_p = NULL,
		.sample_frec = 0};
	analog_input_config_t senial_Humedad_sensor = {
		.input = CH2,
		.mode = ADC_SINGLE,
		.func_p = NULL,
		.param_p = NULL,
		.sample_frec = 0};

	timer_config_t timer_medicion_senial = {
		.timer = TIMER_A,
		.period = CONFIG_BLINK_PERIOD_medicion_US,
		.func_p = FuncTimerA,
		.param_p = NULL};
	timer_config_t informacion_estado = {
		.timer = TIMER_B,
		.period = CONFIG_BLINK_PERIOD_medicion_US_ESTADO,
		.func_p = FuncTimerB,
		.param_p = NULL};
	// configruacion de puerto uart
	serial_config_t conf_puerto = {
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = NULL,
		.param_p = NULL};
	UartInit(&conf_puerto);
	TimerInit(&timer_medicion_senial);
	TimerInit(&informacion_estado);
	AnalogInputInit(&senial_PH_sensor);
	AnalogInputInit(&senial_Humedad_sensor);
	// creo las tareas:
	// tarea medir
	xTaskCreate(&senial_medir_task, "senial_medida", 2048, NULL, 5, &medicion_task_handle);
	// tarea activar bombas:
	xTaskCreate(&Informar_estado_task, "activar_bombas", 2048, NULL, 5, &Informar_task_handle); 
	// interrupciones
	SwitchActivInt(SWITCH_1, &encender_programa, NULL); /*permite que comience el programa*/
	SwitchActivInt(SWITCH_2, &parar_programa, NULL); /*dejo de ver los valores de las mediciones*/
	/* Inicialización del conteo de los timers */
	TimerStart(TIMER_A);
	TimerStart(TIMER_B);
}
/*==================[end of file]============================================*/