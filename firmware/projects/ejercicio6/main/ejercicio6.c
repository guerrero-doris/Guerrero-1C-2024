/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 *El programa implementa una función que controla un display LCD utilizando pines GPIO específicos para cada dígito.
 *La función recibe un valor de 32 bits, lo descompone en sus dígitos individuales y los muestra en el display LCD, siguiendo un mapeo 
 *predefinido entre los dígitos del LCD y los pines GPIO.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |   Display   |   EDU-CIAA	|
 * |:----------:|:-------------:|
 * | 	Vcc     |	5V      	|
 * | 	D1		| 	GPIO_20		|
 * | 	D2	 	| 	GPIO_21		|
 * | 	D3	 	| 	GPIO_22		|
 * | 	D4	 	| 	GPIO_23		|
 * | 	SEL1	| 	GPIO_19		|
 * | 	SEL2	| 	GPIO_18		|
 * | 	SEL3	| 	GPIO_9		|
 * | 	Gnd 	| 	GND     	|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 15/03/2024 | Document creation		                         |
 *
 * @author Guerrero Doris Micaela (doris.guerrero@ingenieria.uner.edu.ar)
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

typedef struct
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;

/**
 * @brief toma el dato recibido, lo descompone en la cantidad de digitos especificada y luego los convierte a BCD, almacenandolos
 * en un arreglo pasado como puntero.
 *  
 * @param data numero.
 * @param digits cantidad de digitos que componen el dato.
 * @param bcd_number arreglo por referencia.
 * @return retorna un cero (0). 
 */
int8_t  convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number) 
{
	for( int8_t i=digits-1; i>=0; i--){
	bcd_number[i]= data % 10;
	data= data/10;
	}
	return(0);
}

/**
 * @brief recibe un dígito en formato BCD (bcd_digit) y un arreglo de estructuras gpioConf_t (gpio_conf)
 * que mapea los bits del dígito a los pines GPIO. La función recorre cada bit del dígito BCD y cambia el estado del pin GPIO correspondiente
 * según los bits del dígito BCD recibido.
 * 
 * @param digito_BCD un digito del cero al nueve (0-9)
 * @param p arreglo de estructura del tipo gpioConf_t.
 * 
 */
void cambio_estado_GPIO(uint8_t digito_BCD, gpioConf_t *p )
{ 
	uint8_t MASK_BIT_1=1; //00000001
	for(int i=0; i<=3; i++){
		if(MASK_BIT_1 & digito_BCD)
		{
				GPIOOn(p[i].pin);
			}
			else{
				GPIOOff(p[i].pin);
			} 
			MASK_BIT_1 = MASK_BIT_1 << 1;
	}
}
/**
 * @brief La función descompone el dato en dígitos individuales utilizando la función convertToBcdArray. Luego, para cada dígito, 
 * cambia el estado de los pines GPIO según el valor del dígito (usando la función cambio_estado_GPIO), enciende el pin correspondiente 
 * al dígito en el LCD (GPIOOn), y finalmente, lo apaga (GPIOOff). Esto permite mostrar el valor completo en el display LCD utilizando 
 * los pines GPIO adecuados y siguiendo el mapeo especificado.
 *
 * @param digito_BCD dato. 
 * @param cant_digitos cantidad de digitos del que esta compuesto el dato.
 * @param pos arreglo de estructura gpioConf_t.
 * @param bcd arreglo de estructura gpioConf_t.
 * 
 */
void mostrar_desplay(uint32_t digito_BCD, uint8_t cant_digitos, gpioConf_t *pos, gpioConf_t *bcd){
	uint8_t arreglo[cant_digitos];
	convertToBcdArray (digito_BCD,cant_digitos, arreglo); 
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

for (int i=0; i<=3; i++){ 
	GPIOInit(pines_bcd[i].pin, pines_bcd[i].dir);
}

//mapeo  de los pines al display
gpioConf_t gpio_pos[3] = {{GPIO_19, GPIO_OUTPUT}, 
							{GPIO_18, GPIO_OUTPUT}, 
					     	{GPIO_9, GPIO_OUTPUT}};
	
for (int i=0; i<=3; i++){ 
	GPIOInit(gpio_pos[i].pin, gpio_pos[i].dir);
}

mostrar_desplay(digito_BCD,cant_digitos, gpio_pos, pines_bcd);

}

/*==================[end of file]============================================*/