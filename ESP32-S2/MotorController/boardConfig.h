/**
 * @file Serial.h
 * @brief Header do Gerenciador de Comunicação Serial 
 * @details Este arquivo contém as definição de configurações de cada 
 * placa de uso na controladora, de modo que seja possível se fazer a 
 * permuta sem alterações no código fonte.
 * @note Usa-se compilação condicional `#ifdef`
 * @author Bruno Gabriel Flores Sampaio,
 * @date 7 de junho de 2024
*/

#ifndef BOARDs_CONFIG_H_
#define BOARDs_CONFIG_H_

// Includes padrão
#include "Arduino.h"

#include "stdbool.h"
#include "WString.h"
#include "stdlib.h"
#include "stdint.h"
#include "stddef.h"
#include "stdio.h"
#include "string"

/* freeRTOS ESP-IDF */
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

/* Drivers do ESP-IDF */
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_adc_cal.h"
#include "driver/adc.h"

#define SENSOR_AS5600_NUM 1 
#define DISPLAY_OLED_NUM  0 

/* Sensor de posição */
#define AS5600_ADDR      0x36
#define ADDRESS_POSITION 0x0C

#define ADC1_CHANNEL     ADC1_CHANNEL_0
#define ADC_ATTEN        ADC_ATTEN_DB_11    
#define ADC_VREF         1100     



/** 
 * @brief Configuração do modo de Debug WiFi 
 * @details Essa macro define o tipo de debug que será usado. 
 *  - O debug via TELNET deverá ser usado em Terminal 
 *  - O debug via HTTP pode ser usado no navegador
 */
#define DEBUG_TELNET 
// #define DEBUG_HTTP 


/** 
 * @brief Mensagem de debug para via WiFi Telnet. 
 * @details É usado um Mutex para não haver conflitos de uso de memórias compartilhadas e deadlocks  D:
 * Para desativar o debug serial, deve ser definido a Macrodefinição DEBUG_DESLIGADO
 * 
 * @example `#define DEBUG_COMPLETO`  -> Printa o nome do arquivo e linha + mensagem de debug
 * @example `#define DEBUG_REDUZIDO`  -> Printa somente a mensagem de debug
 * @example `#define DEBUG_DESLIGADO` -> Desliga o print de mensagens de debug 
 */
// #define DEBUG_COMPLETO 
#define DEBUG_REDUZIDO 
// #define DEBUG_DESLIGADO 


/**
 * @brief GPIO do LED da placa 
 */ 
#define BOARD_LED           ((gpio_num_t)GPIO_NUM_15)


/* GPIOs I2C de leitura do sensor de posição */
#define POS_SCL_PIN         ((gpio_num_t)GPIO_NUM_33)
#define POS_SDA_PIN         ((gpio_num_t)GPIO_NUM_18)

/* GPIOs auxiliares do sensor de posição */
#define ADC_SENSOR_POS      ((gpio_num_t)GPIO_NUM_14)
#define PROG_SENSOR         ((gpio_num_t)GPIO_NUM_13)
#define DIR_SENSOR          ((gpio_num_t)GPIO_NUM_10)


/* GPIOs I2C de escrita no display OLED */
#define DISPLAY_SCL_PIN     ((gpio_num_t)GPIO_NUM_3)
#define DISPLAY_SDA_PIN     ((gpio_num_t)GPIO_NUM_5)


/* GPIOs ADCs de leitura de tensão */
#define BREAK_PIN           ((gpio_num_t)GPIO_NUM_4)
#define PWM_PIN             ((gpio_num_t)GPIO_NUM_2)
#define DIR_PIN             ((gpio_num_t)GPIO_NUM_6)

#endif