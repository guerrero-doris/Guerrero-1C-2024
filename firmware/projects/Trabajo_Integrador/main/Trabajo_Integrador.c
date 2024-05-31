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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "timer_mcu.h"
#include "uart_mcu.h"
#include "analog_io_mcu.h"
#include "gpio_mcu.h"
#include "math.h"
#include "switch.h"
#include "ADXL335.h"
#include "ble_mcu.h"


#define CONFIG_BLINK_PERIOD_medicion_US 1000
TaskHandle_t Lectura_task_handle = NULL;
bool Inicio_Lectura;
bool start = false;
bool ver_datos= false;

float vector_datos_x[1000] = {0}; // Puntero para almacenar los datos del canal CH1
float vector_datos_y[1000] = {0}; // Puntero para almacenar los datos del canal CH2
float vector_datos_z[1000] = {0}; // Puntero para almacenar los datos del canal CH3
int cant_muestras;

float calcular_desvio_estandar(float *vector_datos, int longitud)
{
	float media = 0.0;
	float sumatoria_cuadrados = 0.0;

	// Calcular la media de los datos
	for (int i = 0; i < longitud; i++)
	{
		media += vector_datos[i];
	}
	media /= longitud;
	// Calcular la sumatoria de los cuadrados de las diferencias
	for (int i = 0; i < longitud; i++)
	{
		sumatoria_cuadrados += powf(vector_datos[i] - media, 2);
	}
	// Calcular el desvío estándar
	float desvio_estandar = sqrtf(sumatoria_cuadrados / longitud);
	return desvio_estandar;
}

float encontrar_maximo(float *vector, int longitud)
{
	float maximo = vector[0]; // Suponemos que el primer elemento es el máximo inicialmente
	for (int i = 1; i < longitud; i++)
	{
		if (vector[i] > maximo)
		{
			maximo = vector[i];
		}
	}
	return maximo;
}

int encontrar_minimo(float *vector, int longitud)
{
	float minimo = vector[0]; // Suponemos que el primer elemento es el mínimo inicialmente

	for (int i = 1; i < longitud; i++)
	{
		if (vector[i] < minimo)
		{
			minimo = vector[i];
		}
	}

	return minimo;
}

void FuncTimerA(void *param)
{
    xTaskNotifyGive(Lectura_task_handle); /* Envía una notificación */
}

void lectura_senial_Task(void *pvParameter)
{   
    float valor_x, valor_y, valor_z;
    cant_muestras = 0; // Número actual de muestras capturadas, para tomar las de interes
    float desvioX= 0.0;
    float desvioY= 0.0;
    float desvioZ= 0.0;
    float minX= 0.0;
    float minY= 0.0;
    float minZ=0.0;
    float maxX=0.0;
    float maxY=0.0;
    float maxZ=0.0;
	char msj[20];
   
    while (1)
    { 
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // recibe la notificacion 
        if(start)//implemento la interrupcion para comenzar a tomar las medidas y detener la medicion
        { 
        valor_x = ReadXValue();    
        vector_datos_x[cant_muestras] = valor_x; 
        printf("valor_x %.f \n", valor_x); //si anda
        valor_y = ReadYValue();
        vector_datos_y[cant_muestras] = valor_y;
        printf("valores_y %f \n", valor_y);
        valor_z = ReadZValue();
        vector_datos_z[cant_muestras] = valor_z;
        printf("valores_z %f \n", valor_z);
        cant_muestras++;
         }
        if(ver_datos){
        desvioX=calcular_desvio_estandar(vector_datos_x, cant_muestras);
        desvioY=calcular_desvio_estandar(vector_datos_y, cant_muestras);
        desvioZ=calcular_desvio_estandar(vector_datos_z, cant_muestras);
        maxX=encontrar_maximo(vector_datos_x,cant_muestras);
        maxY=encontrar_maximo(vector_datos_y,cant_muestras);
        maxZ=encontrar_maximo(vector_datos_z,cant_muestras);
        minX=encontrar_minimo(vector_datos_x,cant_muestras);
        minY=encontrar_minimo(vector_datos_y,cant_muestras);
        minZ=encontrar_minimo(vector_datos_z,cant_muestras);
        printf(msj,"desvio_estandarX %.2f, desvio_estandarY %.2f, desvio_estandarZ %.2f,", desvioX,desvioY,desvioZ);
        printf("\n");
        BleSendString(msj);
        printf(msj,"minX %.2f, minY %.2f, minZ %.2f,", minX,minY,minZ);
        printf("\n");
        BleSendString(msj);
        printf(msj,"maxX %.2f, maxY %.2f, maxZ %.2f,", maxX,maxY,maxZ);
        printf("\n");
        BleSendString(msj);
        ver_datos=false;
        }
    }
   
} 
void comienzar_programa()
{
	start = !start;
}
void devolver_medidas(){
    ver_datos=!ver_datos;
}

void app_main(void)
{
    ADXL335Init();
    timer_config_t timer_medicion= {
        .timer = TIMER_A,
        .period = CONFIG_BLINK_PERIOD_medicion_US,
        .func_p = FuncTimerA,
        .param_p = NULL
    };
    TimerInit(&timer_medicion);
     
    //configuracion de bluetooth
    ble_config_t ble_configuration_graficas = {
        "MEDIDOR_DORIS",
         NULL 
    };
     BleInit(&ble_configuration_graficas);
   

    /* Creación de tareas */
    xTaskCreate(&lectura_senial_Task, "senial de entrada", 2048, NULL, 5, &Lectura_task_handle);
    /*creo interrupciones para que tocando un boton inicie o pare las mediciones*/
    SwitchActivInt(SWITCH_1, &comienzar_programa, NULL);
    /*creo interrupcion para ver el calculo estadistico*/
    SwitchActivInt(SWITCH_2, &devolver_medidas, NULL);
      /* Inicialización del conteo del timer */
    TimerStart(timer_medicion.timer);
  
}
/*==================[end of file]============================================*/