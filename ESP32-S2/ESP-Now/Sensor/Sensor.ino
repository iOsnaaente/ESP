#include "../../common/myDebug_serial.h"
#include "../../common/myESP_now.h"

/* RTOS ESP 32 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_now.h"

/* Drivers IDF ESP32*/
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

/* Sensor */
#define sensor_pin GPIO_NUM_34
#define sensor_channel ADC1_CHANNEL_6
#define sensor_unit ADC_UNIT_1
#define sensor_attenuation ADC_ATTEN_DB_12
#define sensor_width ADC_WIDTH_BIT_12
#define sensor_multi_sampling true
#define sensor_num_samplings 10
#define sensor_period 10
int32_t sensor_value = 0;


/* Endereço MAC do receptor */
uint8_t peer_mac_address[6] = { 0x24, 0x6F, 0x28, 0x79, 0x56, 0x98 };


/* Funções Callback do ESP Now */
void on_send_data(const uint8_t *macAddr, esp_now_send_status_t status);

/* Funções RTOS */
void sensor_read_task(void *pvParameters);


void setup() {
  /* Cria o Mutex */
  serialDebuggerMutex = xSemaphoreCreateMutex();
  /* Inicializa o Serial Monitor */
  init_serial();
  SERIAL_PRINT("SENSOR APP", "ESP-NOW Sensor Device Init");
  /* Inicia o ESP Now */
  init_esp_now();
  /* Registra a função de callback para status de envio de dados */
  esp_now_register_send_cb((esp_now_send_cb_t)on_send_data);
  /* Adicionar o Peer do tipo broadcast */
  add_peer(peer_mac_address, 0);

  /* Tasks de funções */
  xTaskCreate(sensor_read_task, "LEITURA_SENSOR", 4096, NULL, 5, NULL);
}


void loop(void) {}


/* Tarefa para ler o sensor LDR */
void sensor_read_task(void *pvParameters) {
  (void)pvParameters;
  /* Configura o sensor como entrada analógica */
  adc1_config_width(sensor_width);
  /* GPIO34 está mapeado para ADC1_CHANNEL_6 */
  adc1_config_channel_atten(sensor_channel, sensor_attenuation);
  /* Cria a curva caracteristica do ESP32 */
  static esp_adc_cal_characteristics_t adc1_chars;
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(sensor_unit, sensor_attenuation, sensor_width, 0, &adc1_chars);
  /* Loop */
  while (true) {
    // Lê o valor do sensor LDR
    uint32_t mean = 0;
    for (uint8_t i = 0; i < sensor_num_samplings; i++) {
      mean += esp_adc_cal_raw_to_voltage(adc1_get_raw(sensor_channel), &adc1_chars);
    }
    sensor_value = uint32_t(mean / sensor_num_samplings);
    PLOTER_PRINT("SENSOR READ [mV]", String(sensor_value));
    /* Envia o valor do sensor para o receptor */
    esp_now_send(peer_mac_address, (uint8_t *)&sensor_value, sizeof(sensor_value));
    vTaskDelay(pdMS_TO_TICKS(sensor_period));
  }
}


/* Callback para status de envio de dados */
void on_send_data(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char mac_str[18];
  format_mac_address(mac_addr, mac_str, 18);
  SERIAL_PRINT("SEND TO", mac_str);
  SERIAL_PRINT("SEND STATUS", status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}