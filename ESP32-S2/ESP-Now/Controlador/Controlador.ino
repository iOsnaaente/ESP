#include "../../common/myDebug_serial.h"
#include "../../common/myESP_now.h"
#include "../../common/myPID.h"

/* RTOS ESP 32 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_now.h"

/* PID Configuration */
pid_controller_t pid = {
    .kp             = 0.75f,
    .ki             = 0.55f,
    .kd             = 0.055f,
    .setpoint       = 1000.0f, // (em mV)
    .integral       = 0.0f,
    .int_sat        = 10000.0f,
    .previous_error = 0.0f,
    .output_min     = 0.0f,
    .output_max     = 8000.0f // Máximo valor (2^13 - 1)
};

/* Variáveis globais para controle */
uint32_t sensor_value = 0;
uint32_t ledc_duty = 0;


/* Endereço MAC do receptor */
uint8_t peers_mac_address[6] = { 0xD4, 0x8A, 0xFC, 0x99, 0x7A, 0xA8 };


/* Funções Callback do ESP Now */
void on_receive_data(const unsigned char *macAddr, const char *incoming_data, int len);
void on_send_data(const uint8_t *macAddr, esp_now_send_status_t status);


void setup() {
  /* Cria o Mutex */
  serialDebuggerMutex = xSemaphoreCreateMutex();
  /* Inicializa o Serial Monitor */
  init_serial();
  SERIAL_PRINT("CONTROLLER APP", "ESP NOW Controller Init");
  /* Inicia o ESP Now */
  init_esp_now();
  /* Registra a função de callback para status de envio de dados */
  esp_now_register_send_cb((esp_now_send_cb_t)on_send_data);
  esp_now_register_recv_cb((esp_now_recv_cb_t)on_receive_data);
  /* Adicionar o Peer do tipo broadcast */
  add_peer(peers_mac_address, 0);
}

void loop() {
}


/* Callback para status de envio de dados */
void on_send_data(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char mac_str[18];
  format_mac_address(mac_addr, mac_str, 18);
  SERIAL_PRINT("SEND TO", mac_str);
  SERIAL_PRINT("SEND STATUS", status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

/* Callback para receber dados */
void on_receive_data(const unsigned char *mac_addr, const char *incoming_data, int len) {
  char mac_str[18] = { 0 };
  format_mac_address(mac_addr, mac_str, 18);
  SERIAL_PRINT("RECEIVED MAC", mac_str);
  SERIAL_PRINT("RECEIVED DATA", String(incoming_data));

  /* Pega o valor do sensor */
  sensor_value = (uint32_t)( \
    (incoming_data[3]<<24) + \
    (incoming_data[2]<<16) + \
    (incoming_data[1]<<8 ) + \
    (incoming_data[0]    )   \
  );

  /* Calcula o PID */
  ledc_duty = (uint32_t)pid_compute(&pid, (double)sensor_value);
  
  /* Envia o valor de duty para o atuador */
  esp_now_send(peers_mac_address, (uint8_t *)&ledc_duty, sizeof(ledc_duty));
  
  PLOTER_PRINT("LEDC_COMPUTED_VALUE", String(ledc_duty));
  PLOTER_PRINT("SENSOR_VALUE", String(sensor_value));
}
