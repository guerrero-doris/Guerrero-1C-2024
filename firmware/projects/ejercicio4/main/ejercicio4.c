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
#include <gpio_mcu.h>
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/


int8_t  convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number) //funcion que cconvierte el dato recibido a BCD
{
	//data = dato de 32 bits
	//digits = cantidad de digitos de salida.
	//puntero a un arreglo  que almacene n_digitos. 
	for( int8_t i=digits-1; i>=0; i--){
	bcd_number[i]= data % 10;
	data= data/10;
	}
	return(0);
}


void app_main(void){
	uint32_t dato;
	dato=123;
	uint8_t digitos;
	digitos=3;
	uint8_t arreglo[digitos];

convertToBcdArray(dato,digitos,arreglo);

for(int i=0; i<=digitos-1; i++)
{
printf("el arreglo es: %u",arreglo[i]);
}
}
/*==================[end of file]============================================*/