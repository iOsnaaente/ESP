#ifndef SERIAL_DEBUG_H_
#define SERIAL_DEBUG_H_

/* FreeRTOS */
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/task.h"

/* Drivers */
#include "driver/uart.h"

/* Logs e auxiliares */
#include "esp_log.h"
#include "string.h"


/* Ativar ou Desativar o Debug Serial  */
// #define SERIAL_DEBUG true
#define PLOTER_DEBUG true

// Definições para UART
#define UART_PORT_NUM UART_NUM_0
#define UART_BAUD_RATE 115200
#define TXD_PIN (UART_PIN_NO_CHANGE)
#define RXD_PIN (UART_PIN_NO_CHANGE)

#define BUF_SIZE      0xff
#define QUEUE_SIZE    0xff


/* Struct to use with serialQueue */
typedef struct {
  uint8_t data[BUF_SIZE];
  uint8_t length;
} serial_queue_data_t;


/* Protótipo das funções de RTOS de leitura UART */
void uart_read_task(void *pvParameters);

/* Cria um Mutex para controle de fluxo no SERIAL_PRINT */
SemaphoreHandle_t serialDebuggerMutex;
/* Fila para armazenar os dados recebidos via Serial */
QueueHandle_t serialQueue;


/* Define Serial Debug */
#ifdef SERIAL_DEBUG
#define SERIAL_PRINT(TIPO, MESSAGE) \
  if (xSemaphoreTake(serialDebuggerMutex, (TickType_t)pdMS_TO_TICKS(100)) == pdTRUE) { \
    char buf[128]; \
    snprintf(buf, sizeof(buf), "[%s] %s\n", TIPO, MESSAGE); \
    uart_write_bytes(UART_PORT_NUM, buf, strlen(buf)); \
    xSemaphoreGive(serialDebuggerMutex); \
  }
#else
#define SERIAL_PRINT(TIPO, MESSAGE) {};
#endif

#ifdef PLOTER_DEBUG
#define PLOTER_PRINT( NAME, VALUE ) \
  if (xSemaphoreTake(serialDebuggerMutex, (TickType_t)pdMS_TO_TICKS(100)) == pdTRUE) { \
    char buf[128]; \
    snprintf(buf, sizeof(buf), "%s:%s\n", NAME, VALUE); \
    uart_write_bytes(UART_PORT_NUM, buf, strlen(buf)); \
    xSemaphoreGive(serialDebuggerMutex); \
  }
#else
#define SERIAL_PRINT(TIPO, MESSAGE) {};
#endif


/* Configura a UART */
esp_err_t init_serial(void) {
  uart_config_t uart_config = {
    .baud_rate = UART_BAUD_RATE,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
  };

  /* Instala e configura a UART driver */
  esp_err_t ret = uart_param_config(UART_PORT_NUM, &uart_config);
  if (ret != ESP_OK) return ret;
  ret = uart_set_pin(UART_PORT_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  if (ret != ESP_OK) return ret;
  ret = uart_driver_install(UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
  if (ret != ESP_OK) return ret;
  SERIAL_PRINT("ESP_SERIAL", "Porta serial iniciada com sucesso!!");

  /* Cria a fila para armazenar os dados recebidos da UART */ 
  serialQueue = xQueueCreate( QUEUE_SIZE, sizeof(serial_queue_data_t) );

  /* Cria a task de leitura serial e envio à serialQueue */
  xTaskCreate( uart_read_task, "UART_READ_SERIAL", 4096, NULL, 5, NULL );

  return ESP_OK;
}

void uart_read_task(void *pvParameters) {
  (void *)pvParameters; 
  // Inicializa as estruturas de dados fila serialQueue 
  uint8_t data[BUF_SIZE];
  serial_queue_data_t buffer = {0};
  // Lê o input da UART
  while ( true ) {
    int len = uart_read_bytes( UART_PORT_NUM, data, BUF_SIZE - 1, 20 / portTICK_RATE_MS );
    if (len > 0) {
      for ( uint8_t i = 0; i < len-1; i++ ) {
        buffer.data[i] = data[i];
      }
      buffer.length = len;  
      buffer.data[len-1] = '\0';  
      // Envia os dados para a fila
      if (xQueueSend(serialQueue, &buffer, portMAX_DELAY) != pdTRUE) {
        SERIAL_PRINT( "QUEUE ERROR", "Falha em enviar os dados para a serialQueue");
      }
      SERIAL_PRINT( "SERIAL RECV", buffer.data);
    }
    vTaskDelay( 10 / portTICK_PERIOD_MS);  
  } 
}


/* Função para obter o último elemento da serialQueue */
esp_err_t read_serial_queue( serial_queue_data_t *buffer ) {
  if ( xQueueReceive( serialQueue, buffer, 10 ) != pdTRUE) {
    return ESP_FAIL;
  }
  return ESP_OK;
}


/* Função para obter o número de elementos na serialQueue */
uint8_t in_waiting_serial_queue( void ) {
    return ( uint8_t ) uxQueueMessagesWaiting( serialQueue );
}


/* Retorna uma String a partir de um buffer de dados - Útil para tratar dados de serial */
String buffer_to_string( const uint8_t* buffer, size_t length, bool characters = true ) {
  String result;
  for (size_t i = 0; i < length; ++i) {
    if ( characters ){
      result += String(buffer[i] );
    }else{
      // Para printar valores   
      if (buffer[i] < 0x10)
        result += "0";
    }
  }
  return result;
}



#endif