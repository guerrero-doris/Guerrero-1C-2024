/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
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
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
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
/*#define CONFIG_BLINK_PERIOD_LED_2 1500
#define CONFIG_BLINK_PERIOD_LED_3 500*/
bool start = true;
bool hold = false;

/*==================[internal data definition]===============================*/
TaskHandle_t tecla_task_handle = NULL;
TaskHandle_t medicion_task_handle = NULL;
/*==================[internal functions declaration]=========================*/

void distanciaTask(void *pvParameter){
    uint16_t distancia;
    while(1)
    {
     ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* La tarea espera en este punto hasta recibir una notificación */
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


void FuncTimerA(void* param){
    xTaskNotifyGive(medicion_task_handle); /* Envía una notificación a la tarea asociada a la medicion */
}

//funcion de interrupcion
void comienzar_programa(){
     start = !start;
}
void hold_medicion(){
      hold=!hold;
}



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
    SwitchActivInt(SWITCH_1, &comienzar_programa, NULL);
    SwitchActivInt(SWITCH_2, &hold_medicion, NULL);
    TimerStart(timer_medicion.timer);

 //  xTaskCreate(&switchTask, "teclas", 512, NULL, 5, &tecla_task_handle);
      /* Inicialización del conteo de timers */
 // TimerStart(timer_tecla.timer);

}
 


/*==================[end of file]============================================*/