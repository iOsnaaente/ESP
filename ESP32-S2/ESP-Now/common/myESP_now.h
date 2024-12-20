#ifndef ESP_NOW_CONFIG_H_
#define ESP_NOW_CONFIG_H_

/* Includes IDF */
#include "esp_wifi_types.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_now.h"
#include "esp_log.h"

/* Includes std */
#include "nvs_flash.h"
#include "string.h"
#include "vector"

/* Includes */
#include "myDebug_serial.h"


/* Endereço Broadcast */
uint8_t broadcast_mac_address[] = { 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF };


/* Estrutura para armazenar informações dos peers */
typedef struct {
  uint8_t mac[6];
  uint8_t channel;
} peer_info_t;

// Vetor para armazenar os peers conectados
std::vector<peer_info_t> peers;

esp_err_t add_peer(const uint8_t *peer_mac, uint8_t channel) {
  esp_now_peer_info_t peerInfo = {0};
  esp_err_t err;

  memcpy(peerInfo.peer_addr, peer_mac, 6);
  peerInfo.channel = channel;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) == ESP_OK) {
    peer_info_t newPeer;
    memcpy(newPeer.mac, peer_mac, 6);
    newPeer.channel = channel;
    peers.push_back(newPeer);

    err = ESP_OK;
    SERIAL_PRINT("ESP-NOW", "Peer added successfully");
  } else {
    err = ESP_FAIL;
    SERIAL_PRINT("ESP-NOW", "Failed to add peer");
  }
  return err;
}

esp_err_t remove_peer(const uint8_t *peer_mac) {
  esp_err_t err = ESP_FAIL;
  if (esp_now_del_peer(peer_mac) == ESP_OK) {
    for (auto it = peers.begin(); it != peers.end(); ++it) {
      if (memcmp(it->mac, peer_mac, 6) == 0) {
        peers.erase(it);
        err = ESP_OK;
        SERIAL_PRINT("ESP-NOW", "Peer removed successfully");
        break;
      }
    }
  } else {
    err = ESP_FAIL;
    SERIAL_PRINT("ESP-NOW", "Failed to remove peer");
  }
  return err;
}

/* Função para formatar um endereço MAC qualquer */
void format_mac_address(const uint8_t *macAddr, char *buffer, int maxLength) {
  snprintf(buffer, maxLength, "%02x:%02x:%02x:%02x:%02x:%02x", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
}

/* Função para obter o endereço MAC do dispositivo */
String get_mac_address(void) {
  uint8_t mac[6];
  char mac_str[18];
  // Obtém o endereço MAC do ESP32
  esp_efuse_mac_get_default(mac);
  // Formata o endereço MAC como string
  format_mac_address(mac, mac_str, 18);
  // Retorna o endereço MAC como String
  return String(mac_str);
}


esp_err_t init_esp_now(void) {
  esp_err_t err = ESP_FAIL;
  // Inicializa a NVS
  nvs_flash_init();
  esp_netif_init();
  esp_event_loop_create_default();
  // Configura o WiFi no modo STA com configuração do WiFi com valores padrão
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_mode(WIFI_MODE_STA);
  err = esp_wifi_start();
  if (err != ESP_OK) {
    SERIAL_PRINT("ERRO WIFI", "WiFi not initialized properly");
    return err;
  } else {
    SERIAL_PRINT("WIFI", "WiFi initialized in station mode (STA)");
  }
  // Inicia o ESP-NOW
  err = esp_now_init();
  if (err != ESP_OK) {
    SERIAL_PRINT("ERROR ESP NOW", "Erro ao iniciar o ESP Now!!");
    return err;
  } else {
    SERIAL_PRINT("ESP NOW", "ESP Now iniciado com sucesso!!");
  }

  /* Adiciona o peer de BROADCAST */
  err = add_peer(broadcast_mac_address, 0);
  return err;
}

#endif  // ESP_NOW_CONFIG_H_
