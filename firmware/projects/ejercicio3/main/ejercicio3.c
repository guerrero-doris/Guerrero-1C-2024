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
struct leds
{
	uint8_t n_led;      //indica el número de led a controlar
	uint8_t n_ciclos;   //indica la cantidad de ciclos de encendido/apagado
	uint8_t periodo;    //indica el tiempo de cada ciclo
	uint8_t mode;       //ON, OFF, TOGGLE
} my_leds;

void definicion(struct leds *led){

}


void app_main(void){

struct leds my_leds;
definicion(&my_leds);

}
/*==================[end of file]============================================*/