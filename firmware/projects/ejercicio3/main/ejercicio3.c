/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *

==================[inclusions]=============================================*/


#include <stdio.h>
#include <stdint.h>
#include "led.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#define CONFIG_BLINK_PERIOD 1000; // configuracion del periodo
struct leds
{
	uint8_t n_led;      //indica el número de led a controlar
	uint8_t n_ciclos;   //indica la cantidad de ciclos de encendido/apagado
	uint16_t periodo;    //indica el tiempo de cada ciclo
	uint8_t mode;       //ON, OFF, TOGGLE
} my_leds;

enum led_mode {on, off, toggle};

void definicion(struct leds *led){
switch (led->mode){
	case on:
	if (led->n_led == LED_1)
		LedOn(LED_1);
	if (led->n_led == LED_2)
		LedOn(LED_2);
	if (led->n_led == LED_3)
		LedOn(LED_3);
	break;

	case off:
	if (led->n_led == LED_1)
		LedOff(LED_1);
	if (led->n_led == LED_2)
		LedOff(LED_2);
	if (led->n_led == LED_3)
		LedOff(LED_3);
	break;

	case toggle:
	for(uint8_t i=0; i<led->n_ciclos; i++)
	{
		if(led->n_led == LED_1)
		{
			LedToggle(LED_1);
			vTaskDelay(led->periodo/ portTICK_PERIOD_MS);
		}
		else if(led->n_led == LED_2)
		{
			LedToggle(LED_2);
			vTaskDelay(led->periodo/ portTICK_PERIOD_MS);
		}
		else if(led->n_led == LED_3)
		{
			LedToggle(LED_3);
			vTaskDelay(led->periodo/ portTICK_PERIOD_MS);
		}
	}
	break;
	}

}

void app_main(void){
LedsInit();

struct leds led;
	
	led.n_led = LED_2;
	led.mode = toggle;
	led.periodo = 500; 
	led.n_ciclos = 10; 
definicion(&led);
while(1)
{
	
}

}
/*==================[end of file]============================================*/