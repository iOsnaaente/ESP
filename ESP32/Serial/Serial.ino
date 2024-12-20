/* FreeRTOS */
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

/* Drivers */
#include "driver/uart.h"

/* Logs e auxiliares */
#include "esp_log.h"
#include "string.h"


// Definições para UART
#define UART_PORT_NUM UART_NUM_0
#define UART_BAUD_RATE 115200
#define BUF_SIZE 1024
#define TXD_PIN (UART_PIN_NO_CHANGE)
#define RXD_PIN (UART_PIN_NO_CHANGE)


/* Define Serial Debug */
#define SERIAL_DEBUG true
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


/* Cria um Mutex para controle de fluxo no SERIAL_PRINT */
SemaphoreHandle_t serialDebuggerMutex;

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
  return ESP_OK;
}


void setup(void) {
  /* Cria o Mutex */
  serialDebuggerMutex = xSemaphoreCreateMutex();
  esp_err_t err = init_serial();
}

const char *message = "HELLO SERIAL\n";

void loop(void) {
  uint32_t len = uart_write_bytes(UART_PORT_NUM, message, strlen(message));
  vTaskDelay(1000 / portTICK_PERIOD_MS);  // Aguarda 1 segundo
}
