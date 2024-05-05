/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * El código controla un sistema que mide distancias con un sensor ultrasónico HC-SR04 y muestra los resultados en un LCD.
 * Además, enciende y apaga LEDs según la distancia medida. Dos botones (switches) controlan la activación y la retención de la medición.
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
 * |   Date	 | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/05/2024 | Document creation		                         |
 *
 * @author  Doris Micaela Guerrero (doris.guerrero@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "gpio_mcu.h"
#include "hc_sr04.h"
#include "lcditse0803.h"
#include "switch.h"
#include "timer_mcu.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 1000
#define CONFIG_BLINK_PERIOD_1 50
#define CONFIG_BLINK_PERIOD_medicion_US 1000000
bool start = true;
bool hold = false;
/*==================[parte retirada del ejercicio 1]==========================*/
/*#define CONFIG_BLINK_PERIOD_LED_2 1500
#define CONFIG_BLINK_PERIOD_LED_3 500*/

/*==================[internal data definition]===============================*/
TaskHandle_t tecla_task_handle = NULL;
TaskHandle_t medicion_task_handle = NULL;
/*==================[internal functions declaration]=========================*/
/**
 * @brief Tarea encargada de medir la distancia, espera una notificación para comenzar la medición.
 */
void distanciaTask(void *pvParameter){
    uint16_t distancia;
    while(1)
    {
     ulTaskNotifyTake(pdTRUE, portMAX_DELAY); 
        if(start)
        {
         distancia=HcSr04ReadDistanceInCentimeters(); 
        if(!hold)
         {
        LcdItsE0803Write(distancia);
         }  
       if(distancia<10){
            LedOff(LED_1);
            LedOff(LED_2);
            LedOff(LED_3);
       }
       else if(distancia<20){
            LedOn(LED_1);
            LedOff(LED_2);
            LedOff(LED_3);
       }
       else if(distancia<30){
            LedOn(LED_1);
            LedOn(LED_2);
            LedOff(LED_3);
       }
       else if(distancia>30){
            LedOn(LED_1);
            LedOn(LED_2);
            LedOn(LED_3);
       }
       }
        else {
             LedsOffAll();
             LcdItsE0803Off();
        }
       
    }
}
/**
 * @brief envía una notificación a la tarea de medición para iniciar una nueva medición periódicamente.
 */
void FuncTimerA(void* param){
    xTaskNotifyGive(medicion_task_handle); /* Envía una notificación a la tarea asociada a la medicion */
}

/**
 * @brief cambia el valor de la variable global 
 */
void comienzar_programa(){
     start = !start;
}
/**
 * @brief cambia el valor de la variable global 
 */
void hold_medicion(){
      hold=!hold;
}


/*==================[parte retirada del ejercicio 1]==========================*/
/*void FuncTimerA(void* param){
    xTaskNotifyGive(tecla_task_handle);   Envía una notificación a la tarea asociada a la tecla 
}*/

/*void switchTask(void *pvParameter){
int8_t teclas;
    while(1){
    	teclas  = SwitchesRead();
    	switch(teclas){
    		case SWITCH_1:
    		    start = !start;
    		break;
    		case SWITCH_2:
                hold=!hold;
    		break;
    	}
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  La tarea espera en este punto hasta recibir una notificación 
	}
}
*/


/*==================[external functions definition]==========================*/
void app_main(void){

    LedsInit();
    LcdItsE0803Init();
    SwitchesInit();
    HcSr04Init(GPIO_3, GPIO_2);

    timer_config_t timer_medicion= {
        .timer = TIMER_A,
        .period = CONFIG_BLINK_PERIOD_medicion_US,
        .func_p = FuncTimerA,
        .param_p = NULL
    };
    TimerInit(&timer_medicion);
      /* Creación de tareas */
    xTaskCreate(&distanciaTask, "Distancia", 2048, NULL, 5, &medicion_task_handle);
     /* Creación de interrupciones */
    SwitchActivInt(SWITCH_1, &comienzar_programa, NULL);
    SwitchActivInt(SWITCH_2, &hold_medicion, NULL);
     /* Inicialización del conteo del timer */
    TimerStart(timer_medicion.timer);
/*==================[parte retirada del ejercicio 1]==========================*/
 //  xTaskCreate(&switchTask, "teclas", 512, NULL, 5, &tecla_task_handle);
      /* Inicialización del conteo de timers */
 // TimerStart(timer_tecla.timer);

}
 


/*==================[end of file]============================================*/