/*! @mainpage Blinking switch
 *
 * \section genDesc General Description
 *
 * This example makes LED_1 and LED_2 blink if SWITCH_1 or SWITCH_2 are pressed.
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
#include <stdbool.h>
#include "freertos/FreeRTOS.h" //para el sistema operativo
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 1000
/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
void app_main(void){
	uint8_t teclas; // libreria inecoboca, variable entera de 8 bits (libreria std int) u: no tiene signo. int()_t o uint()_t
	LedsInit(); //inicializacion de las teclas
	SwitchesInit(); //inicializacion de las teclas
	//teclas  = SwitchesRead(); //funcion de lectura de los leds, este ya tiene configurado el switch 1, 2, 3. 
    while(1)    {
		teclas  = SwitchesRead(); //funcion de lectura de los leds, este ya tiene configurado el switch 1, 2, 3. 
    	//teclas  = SwitchesRead(); //funcion de lectura de los leds, este ya tiene configurado el switch 1, 2, 3. 
		//carga una variable de 8 bis, con las ultimas variables encendiod o apagado carga los ultimos 2 bits determina si esta encedido o apagado
    	switch(teclas){
    		case SWITCH_1:
    			LedToggle(LED_1); //invierte el estado del led 1 cuando se mantenga apretado el boton 1
    		break;
    		case SWITCH_2:
    			LedToggle(LED_2);
    		break;
			case (SWITCH_2|SWITCH_1):
				LedToggle(LED_3);		
			break;
    	}
	   // LedToggle(LED_3); // cambia de estado de manera continua con un delay de cierto tiempo.
		vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS); // estado de no funcional durante el tiempo.
	}
}

