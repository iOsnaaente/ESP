/**
 * @file WiFiDebugger.cpp
 * @brief Debugger WiFi usando TelNet  
 * @author Bruno Gabriel Flores Sampaio,
 * @date 6 de abril de 2024
 */


#include "WiFiDebugger.h"
#include "../boardConfig.h"


/* Semafaro para controle de fluxo de dados */
SemaphoreHandle_t wifi_debugger_Mutex;

/* Credenciais WiFi */
const char* SSID_TELNET_WIFI = "TauraBots";
const char* PSD_TELNET_WIFI = "robotica2024";


#ifdef DEBUG_HTTP
    // Servidor que irá hospedar o debug 
    WebServer wifi_server(80);    
    String debug_messages[MAX_LINES];  
    uint8_t debug_index = 0;
#endif 

#ifdef DEBUG_TELNET
    // Servidor que irá hospedar o debug
    WiFiServer wifi_server;    
    // Cliente que irá escrever no debug 
    WiFiClient wifi_client;    
#endif 


void wifi_debugger_init( void ) {
    
    /* Inicia o mutex para controle do DEBUG_WIFI */ 
    wifi_debugger_Mutex = xSemaphoreCreateMutex();
    
    /* Inicia a interface WiFi */ 
    WiFi.begin(SSID_TELNET_WIFI, PSD_TELNET_WIFI);
    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    /* Inicia o servidor TelNet para publicação DEBUG */ 
    #ifdef DEBUG_TELNET 
        wifi_server.begin(26);
        wifi_server.setNoDelay(true);
        DEBUG_WIFI( "TELNET", "Servidor TelNet inicializado com sucesso!" );
    #endif 

    #ifdef DEBUG_HTTP
        wifi_server.on("/", debug_page_Handle );
        wifi_server.begin();
        xTaskCreate( server_handle_Task, "WiFi_debugger_Task", 1024*8, NULL, 1, NULL );
    #endif 
}

String buffer2String(const uint8_t* buffer, size_t length) {
    String result;
    for (size_t i = 0; i < length; ++i) {
        if (buffer[i] < 0x10)
            result += "0";
        result += String(buffer[i], HEX);
    }
    return result;
}

#ifdef DEBUG_HTTP
    // Página que será desenhada no Debugger
    void debug_page_Handle() {
        String html = "<html><head><meta http-equiv='refresh' content='1'></head><body>";
        html += "<h2>Debug ESP32-S2</h2><pre>";
        // Escreve as mensagens personalizadas 
        for (int i = 0; i < MAX_LINES; i++) {
            int index = (debug_index + i) % MAX_LINES;
            if (debug_messages[index].length() > 0) {
                html += debug_messages[index] + "\n";
            }
        }
        html += "</pre></body></html>";
        wifi_server.send(200, "text/html", html);
    }

    /* Cria uma Task freeRTOS para poder lidar com os handles periodicos */
    void server_handle_Task( void *pvParameter ){
        while ( true ){
            wifi_server.handleClient();
            vTaskDelay( pdMS_TO_TICKS(250) );
        }
        vTaskDelete(NULL);
    }
#endif