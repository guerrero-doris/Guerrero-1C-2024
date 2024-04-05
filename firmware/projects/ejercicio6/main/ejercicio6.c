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

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/

/*Escriba una función que reciba un dato de 32 bits,  la cantidad de dígitos de salida y dos vectores
de estructuras del tipo  gpioConf_t. Uno  de estos vectores es igual al definido en el punto anterior
y el otro vector mapea los puertos con el dígito del LCD a donde mostrar un dato:
Dígito 1 -> GPIO_19
Dígito 2 -> GPIO_18
Dígito 3 -> GPIO_9

La función deberá mostrar por display el valor que recibe. Reutilice las 
funciones creadas en el punto 4 y 5. Realice la documentación de este ejercicio usando Doxygen.*/
typedef struct
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;
int8_t  convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number) //funcion que cconvierte el dato recibido a BCD
{
	for( int8_t i=digits-1; i>=0; i--){
	bcd_number[i]= data % 10;
	data= data/10;
	}
	return(0);
}

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
}

void mostrar_desplay(uint32_t digito_BCD, uint8_t cant_digitos, gpioConf_t *pos, gpioConf_t *bcd){
	//digito BCD es el que entra del punto 4 ejemplo 123
	uint8_t arreglo[cant_digitos];
	convertToBcdArray (digito_BCD,cant_digitos, arreglo); //aca ya tengo la division del digito en cada uno de los lugares del vector 
	for( uint8_t i=0; i<4; i++){
		cambio_estado_GPIO(arreglo[i], bcd);
		GPIOOn(pos[i].pin);
		GPIOOff(pos[i].pin);
	}

}

void app_main(void){
uint32_t digito_BCD=140;
uint8_t cant_digitos=3;
gpioConf_t pines_bcd[4] = {{GPIO_20, GPIO_OUTPUT}, 
							{GPIO_21, GPIO_OUTPUT}, 
					     	{GPIO_22, GPIO_OUTPUT}, 
							{GPIO_23, GPIO_OUTPUT}};

for (int i=0; i<=3; i++){ //creo que asi inicializaria los pines
	GPIOInit(pines_bcd[i].pin, pines_bcd[i].dir);
}

//mapeo  de los pines al display
gpioConf_t gpio_pos[3] = {{GPIO_19, GPIO_OUTPUT}, 
							{GPIO_18, GPIO_OUTPUT}, 
					     	{GPIO_9, GPIO_OUTPUT}};
	
for (int i=0; i<=3; i++){ //creo que asi inicializaria los pines de salida para el display
	GPIOInit(gpio_pos[i].pin, gpio_pos[i].dir);
}

mostrar_desplay(digito_BCD,cant_digitos, gpio_pos, pines_bcd);

}

/*==================[end of file]============================================*/