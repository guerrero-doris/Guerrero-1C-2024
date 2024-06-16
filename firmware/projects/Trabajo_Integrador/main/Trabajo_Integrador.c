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
 * |  Acelerometro  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	EJE_X	 	| 	GPIO_1		|
 * |    EJE_Y       |   GPIO_2      |
 * |    EJE_Z       |   GPIO_3      |
 * |    gs1         |   GND         |
 * |    gs2         |   3,3 v       |
 * |    Vcc         |   3,3 v       |
 * |    GND         |   GND         |
 * |    SM          |   3,3 v       |
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Guerrero, Doris Micaela (doris.guerrero@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "timer_mcu.h"
#include "uart_mcu.h"
#include "analog_io_mcu.h"
#include "gpio_mcu.h"
#include "math.h"
#include "switch.h"
#include "ble_mcu.h"
#include "ADXL335.h"
#include "string.h"
#include "neopixel_stripe.h"

#define CONFIG_BLINK_PERIOD_medicion_US 3000
#define CONFIG_BLINK_PERIOD_estado_US 2000
TaskHandle_t Lectura_task_handle = NULL;
TaskHandle_t verificacion_task_handle = NULL;
bool Inicio_Lectura;
bool start = false;
bool ver_datos;
bool calibrar = false;

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
void FuncTimerB(void *param)
{
    xTaskNotifyGive(verificacion_task_handle); /* Envía una notificación */
}

void lectura_senial_Task(void *pvParameter)
{
    float valor_x, valor_y, valor_z;
    cant_muestras = 0; // Número actual de muestras capturadas, para tomar las de interes
    float desvioX = 0.0;
    float desvioY = 0.0;
    float desvioZ = 0.0;
    float minX = 0.0;
    float minY = 0.0;
    float minZ = 0.0;
    float maxX = 0.0;
    float maxY = 0.0;
    float maxZ = 0.0;
    char msj[20];
    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // recibe la notificacion
        if (start)                               // implemento la interrupcion para comenzar a tomar las medidas y detener la medicion
        {
            valor_x = ReadXValue();
            vector_datos_x[cant_muestras] = valor_x;
            printf("valores_x %f \n", valor_x);
            sprintf(msj, "*X%.2f*", valor_x);
            BleSendString(msj);
            valor_y = ReadYValue();
            printf("\n");
            vector_datos_y[cant_muestras] = valor_y;
            printf("valores_y %f \n", valor_y);
            sprintf(msj, "*Y%.2f*", valor_y);
            BleSendString(msj);
            valor_z = ReadZValue();
            vector_datos_z[cant_muestras] = valor_z;
            printf("valores_z %f \n", valor_z);
            cant_muestras++;
        }
        if (ver_datos)
        {
            desvioX = calcular_desvio_estandar(vector_datos_x, cant_muestras);
            sprintf(msj, "G%.2f*", desvioX);
            sprintf(msj, "*G%.2f*", desvioX);
            BleSendString(msj);
            desvioY = calcular_desvio_estandar(vector_datos_y, cant_muestras);
            sprintf(msj, "H%.2f*", desvioY);
            sprintf(msj, "*H%.2f*", desvioY);
            BleSendString(msj);
            desvioZ = calcular_desvio_estandar(vector_datos_z, cant_muestras);
            sprintf(msj, "I%.2f*", desvioZ);
            sprintf(msj, "*I%.2f*", desvioZ);
            BleSendString(msj);
            maxX = encontrar_maximo(vector_datos_x, cant_muestras);
            sprintf(msj, "A%.2f*", maxX);
            sprintf(msj, "*A%.2f*", maxX);
            BleSendString(msj);
            maxY = encontrar_maximo(vector_datos_y, cant_muestras);
            sprintf(msj, "B%.2f*", maxY); // este
            sprintf(msj, "*B%.2f*", maxY);
            BleSendString(msj);
            maxZ = encontrar_maximo(vector_datos_z, cant_muestras);
            sprintf(msj, "C%.2f*", maxZ);
            sprintf(msj, "*C%.2f*", maxZ);
            BleSendString(msj);
            minX = encontrar_minimo(vector_datos_x, cant_muestras);
            sprintf(msj, "D%.2f*", minX);
            sprintf(msj, "*D%.2f*", minX);
            BleSendString(msj);
            minY = encontrar_minimo(vector_datos_y, cant_muestras);
            sprintf(msj, "E%.2f*", minY);
            sprintf(msj, "*E%.2f*", minY);
            BleSendString(msj);
            minZ = encontrar_minimo(vector_datos_z, cant_muestras);
            sprintf(msj, "F%.2f*", minZ);
            sprintf(msj, "*F%.2f*", minZ);
            BleSendString(msj);
        }
    }
}
void calibracion()
{
    if (calibrar == true)
    {
        ADXL335Calibration();
    }
}

void Verificacion_estado_Task(void *pvParameter)
{
    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        switch (BleStatus())
        {
        case BLE_OFF:
            NeoPixelAllOff();
            break;
        case BLE_DISCONNECTED:
            NeoPixelAllColor(NEOPIXEL_COLOR_RED);
            break;
        case BLE_CONNECTED:
            NeoPixelAllColor(NEOPIXEL_COLOR_ROSE);
            break;
        }
    }
}
void read_data(uint8_t *data, uint8_t length)
{
    switch (data[0])
    {
    case 'R':
        start = true;
        break;
    case 'r':
        start = false;
        break;
    case 'B':
        ver_datos = true;
        break;
    case 'b':
        ver_datos = false;
        break;
    case 'C':
        calibrar = true;
        break;
    case 'c':
        calibrar = false;
    }
}

void app_main(void)
{
    static neopixel_color_t color;
    ADXL335Init();
    timer_config_t timer_medicion = {
        .timer = TIMER_A,
        .period = CONFIG_BLINK_PERIOD_medicion_US,
        .func_p = FuncTimerA,
        .param_p = NULL};
    TimerInit(&timer_medicion);

    timer_config_t timer_estado = {
        .timer = TIMER_B,
        .period = CONFIG_BLINK_PERIOD_estado_US,
        .func_p = FuncTimerB,
        .param_p = NULL};
    TimerInit(&timer_estado);
    // configuracion de bluetooth
    ble_config_t ble_configuration_graficas = {
        "MEDIDOR",
        read_data};

    NeoPixelInit(BUILT_IN_RGB_LED_PIN, BUILT_IN_RGB_LED_LENGTH, &color);
    NeoPixelAllOff();
    BleInit(&ble_configuration_graficas);
    /* Creación de tareas */
    xTaskCreate(&lectura_senial_Task, "senial de entrada", 2048, NULL, 5, &Lectura_task_handle);
    xTaskCreate(&Verificacion_estado_Task, "Estado de bloetoth", 2048, NULL, 5, &verificacion_task_handle);
    /* Inicialización del conteo del timer */
    TimerStart(timer_medicion.timer);
    TimerStart(timer_estado.timer);
}
/*==================[end of file]============================================*/