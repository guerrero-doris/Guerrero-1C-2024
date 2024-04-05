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
#include "gpio_mcu.h"
/*==================[macros and definitions]=================================*/

typedef struct
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
void cambio_estado_GPIO(uint8_t digito_BCD, gpioConf_t *p )// función que reciba como parámetro un dígito BCD y un vector de estructuras del tipo gpioConf_
{ 
	uint8_t MASK_BIT_1=1; //00000001
	for(int i=0; i<=3; i++){
		if(MASK_BIT_1 & digito_BCD)
		{//00000001 la mascara con un 1 en el bit menos significativo
				GPIOOn(p[i].pin);
			}
			else{
				GPIOOff(p[i].pin);
			} 
			MASK_BIT_1 = MASK_BIT_1 << 1;
	}
		/*if((MASK_BIT_1 << 1) & digito_BCD)
		{//00000010
				GPIOOn(p[i].pin);
			}
			else GPIOOff(p[i].pin);
		if((MASK_BIT_1 << 2) & digito_BCD) 
		{//00000100
				GPIOOn(p[i].pin);
			}
			else GPIOOff(p[i].pin);

		if((MASK_BIT_1 << 3) & digito_BCD)
		{//00001000
				GPIOOn(p[i].pin);
			}
			else GPIOOff(p[i].pin);
		} */
}

/*La función deberá cambiar el estado de cada GPIO,
 a ‘0’ o a ‘1’, según el estado del bit correspondiente en el BCD ingresado. 
 Ejemplo: b0 se encuentra en ‘1’, el estado de GPIO_20 debe setearse. */
 //como estrategia para resolverlo usare mascaras
 
/*==================[external functions definition]==========================*/
void app_main(void){

gpioConf_t pines_bcd[4] = {{GPIO_20, GPIO_OUTPUT}, 
							{GPIO_21, GPIO_OUTPUT}, 
					     	{GPIO_22, GPIO_OUTPUT}, 
							{GPIO_23, GPIO_OUTPUT}};

for (int i=0; i<=3; i++){ //creo que asi inicializaria los pines
	GPIOInit(pines_bcd[i].pin, pines_bcd[i].dir);
}
cambio_estado_GPIO(2,pines_bcd); //pasando pines_bcd paso al puntero al primer lugar del vector

}
/*==================[end of file]============================================*/

