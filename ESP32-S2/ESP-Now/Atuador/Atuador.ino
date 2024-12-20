#include "../../common/myDebug_serial.h"
#include "../../common/myESP_now.h"

/* RTOS ESP 32 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_now.h"

/* Drivers IDF ESP32*/
#include "driver/gpio.h"
#include "driver/ledc.h"

/* Led blink */
#define ledc_timer LEDC_TIMER_0
#define ledc_mode LEDC_HIGH_SPEED_MODE
#define ledc_pin GPIO_NUM_2
#define ledc_channel LEDC_CHANNEL_0
#define ledc_duty_res LEDC_TIMER_13_BIT
#define ledc_freq 5000
#define ledc_tperiod 10

uint32_t ledc_duty = 0;

/* Funções Callback do ESP Now */
void on_receive_data(const unsigned char *macAddr, const char *incoming_data, int len);


void setup() {
  /* Cria o Mutex */
  serialDebuggerMutex = xSemaphoreCreateMutex();
  /* Inicializa o Serial Monitor */
  init_serial();
  SERIAL_PRINT("ATUADOR APP", "ESP-NOW Actuator Device Init");
  /* Inicia o ESP Now */
  init_esp_now();
  /* Registra a função de callback para status de envio de dados */
  esp_now_register_recv_cb((esp_now_recv_cb_t)on_receive_data);

  // Configurar LEDC Timmer e Channel 
  ledc_timer_config_t ledc_timer_params = {
    .speed_mode = ledc_mode,
    .duty_resolution = ledc_duty_res,
    .timer_num = ledc_timer,
    .freq_hz = ledc_freq,
    .clk_cfg = LEDC_AUTO_CLK,
  };
  ledc_channel_config_t ledc_channel_params = {
    .gpio_num = ledc_pin,
    .speed_mode = ledc_mode,
    .channel = ledc_channel,
    .intr_type = LEDC_INTR_DISABLE,
    .timer_sel = ledc_timer,
    .duty = 0,
    .hpoint = 0,
  };
  /* Coloca as configurações*/
  ledc_timer_config(&ledc_timer_params);
  ledc_channel_config(&ledc_channel_params);

}

void loop() {
    // PLOTER_PRINT("DUTY OUT", String(ledc_duty) );
    ledc_set_duty(ledc_mode, ledc_channel, ledc_duty);
    ledc_update_duty(ledc_mode, ledc_channel);
    vTaskDelay(pdMS_TO_TICKS(25));
}


/* Callback para receber dados */
void on_receive_data(const unsigned char *mac_addr, const char *incoming_data, int len) {
  char mac_str[18] = { 0 };
  format_mac_address(mac_addr, mac_str, 18);
  SERIAL_PRINT("RECEIVED MAC", mac_str);
  SERIAL_PRINT("RECEIVED DATA", String(incoming_data));
  ledc_duty = (uint32_t)(    \
    (incoming_data[3]<<24) + \
    (incoming_data[2]<<16) + \
    (incoming_data[1]<<8 ) + \
    (incoming_data[0]    )   \
  );
  PLOTER_PRINT("DUTY OUT", String(ledc_duty) );
}