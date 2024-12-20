#include "esp_idf_version.h"

#include "../common/DebugSerial.h"
#include "../common/ESPNow.h"


#define PAIR_LED 2


// Função Callback de recebimento de pacotes
void on_receive_data(const unsigned char *macAddr, const char *incoming_data, int len);


void setup(void) {
  /* Cria o Mutex */
  serialDebuggerMutex = xSemaphoreCreateMutex();
  /* Inicializa o Serial Monitor */
  init_serial();
  SERIAL_PRINT("ESP NOW", "ESP-NOW Sender");
  /* Inicia o ESP Now */
  init_esp_now();
  /* Registra a função de callback para status de envio de dados */
  esp_now_register_send_cb((esp_now_send_cb_t)on_send_data);
  esp_now_register_recv_cb((esp_now_recv_cb_t)on_receive_data);
  /* Adiciona o BROADCAST PEER na lisa de Peers */
  add_peer(broadcast_mac_address, 0);

}

void loop(void) {}


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
  SERIAL_PRINT("RECEIVED DATA", String( incoming_data) );
  
  // Envia a mensagem para o remetente
  esp_now_send(broadcast_mac_address, (const uint8_t *)incoming_data, len);
}
