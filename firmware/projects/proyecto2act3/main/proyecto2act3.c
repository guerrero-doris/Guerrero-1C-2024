/*! @mainpage Template
 *
 * @section genDesc General Description
 * Integra diferentes componentes (sensor, LCD, LEDs, switches, UART) y los controla de manera coordinada. Permite 
 * medir distancias, mostrar resultados, controlar acciones con interruptores y recibir comandos por UART.
 * 
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 26/05/2024 | Document creation		                         |
 *
 * @author Doris Micaela Guerrero (doris.guerrero@ingenieria.uner.edu.ar)
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
#include "uart_mcu.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 1000
#define CONFIG_BLINK_PERIOD_1 50
#define CONFIG_BLINK_PERIOD_medicion_US 1000000

bool start = true;
bool hold = false;
#define UART_NO_INT	0 //dudoso
/*==================[internal data definition]===============================*/
TaskHandle_t tecla_task_handle = NULL;
TaskHandle_t medicion_task_handle = NULL;

/**
 * @brief Tarea encargada de medir la distancia. Espera una notificación para comenzar la medición, una vez recibida mide. El valor medido 
 * lo convierte en una cadena de caracteres junto con la unidad "cm" seguida de un salto de línea.
 */
void distanciaTask(void *pvParameter){
    uint16_t distancia;
    while(1)
    {
     ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* La tarea espera en este punto hasta recibir una notificación */
        if(start)
        {
         distancia=HcSr04ReadDistanceInCentimeters(); 
         UartSendString(UART_PC,(char*)UartItoa(distancia,10));
         UartSendString(UART_PC,"cm \r\n");
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
 * @brief envia una notificacion a la tarea para que realice una nueva medición.
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

/**
 * @brief  lee un byte del puerto serie UAR y dependiendo del valor recibido ('O' o 'H'), ejecuta una acción específica.
 */
void lectura_teclas(){
uint8_t teclas;
UartReadByte(UART_PC, &teclas);
    	switch(teclas){
    		case 'O':
    			 comienzar_programa();
    		break;
    		case 'H':
    			 hold_medicion();
    		break;
    	}
}

/*==================[external functions definition]==========================*/
void app_main(void){

    LedsInit();
    LcdItsE0803Init();
    SwitchesInit();
    HcSr04Init(GPIO_3, GPIO_2);

	serial_config_t conf_puerto= {
		.port= UART_PC,
		.baud_rate= 9600,
		.func_p= &lectura_teclas,
		.param_p= NULL,
	};
     UartInit(&conf_puerto);
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
    

}

 


/*==================[end of file]============================================*/