/**
 * @file SerialDebugger.h
 * @brief Header do Debugger Serial  
 * @details Este arquivo contém as definição e prototipos das implementações
 *        - de debug serial utilizando a UART0. Para utilizar o debug é
 *        - necessário se chamar a Macro DEBUG_SERIAL( TIPO, MESSAGE )  
 * @author Bruno Gabriel Flores Sampaio,
 * @date 6 de abril de 2024
 */


#ifndef WIFI_DEBUGGER_H_
#define WIFI_DEBUGGER_H_

#include "../boardConfig.h"

#include "WiFi.h"

#include "WiFiClient.h"
#include "WiFiServer.h"
#include "WebServer.h"


extern const char* SSID_TELNET_WIFI;
extern const char* PSD_TELNET_WIFI;


/* Mutex para proteger o acesso à escrita via Telnet */
extern SemaphoreHandle_t wifi_debugger_Mutex;

/** 
 * @brief Objetos de acesso ao servidor Debugger HTTP
 * @details Ao contrário do TelNet, o servidor HTTP irá printar 
 * em uma tela web pela porta 80, um HTML personalizado com os 
 * prints requisitados.
 * @note Isso irá requisistar mais do sistema. 
 * @note O servidor HTML pode ser personalizado  
**/
#ifdef DEBUG_HTTP
    // Número máximo de linhas de debug
    #define MAX_LINES 20  

    // Servidor que irá hospedar o debug 
    extern WebServer wifi_server;    
    extern String debug_messages[MAX_LINES];  
    extern uint8_t debug_index;

    #ifdef DEBUG_COMPLETO
    #define DEBUG_WIFI( TIPO, MESSAGE ) \
        debug_messages[debug_index] = "[" + String( TIPO) + "] " + String(__FILE__) + " [" + String(__LINE__) + "]: " + MESSAGE + "\n"; \
        debug_index = (debug_index+1) % MAX_LINES;
    #endif

    #ifdef DEBUG_REDUZIDO
    #define DEBUG_WIFI( TIPO, MESSAGE ) \
        debug_messages[debug_index] =  String("[") + String(TIPO) + "]: " + MESSAGE + String("\r\n"); \
        debug_index = (debug_index+1) % MAX_LINES;
    #endif 

    // Página que será desenhada no Debugger
    void debug_page_Handle();

    /* Cria uma Task freeRTOS para poder lidar com os handles periodicos */
    void server_handle_Task( void *pvParameter );
#endif


/* Objetos de acesso ao servidor Debugger Telnet  */
#ifdef DEBUG_TELNET
    // Servidor que irá hospedar o debug
    extern WiFiServer wifi_server;    
    // Cliente que irá escrever no debug 
    extern WiFiClient wifi_client;    

    #ifdef DEBUG_COMPLETO
    #define DEBUG_WIFI( TIPO, MESSAGE ) \
        if (xSemaphoreTake(wifi_debugger_Mutex, ( TickType_t ) pdMS_TO_TICKS(10) ) == pdTRUE) { \
            String log_message = "[" + String( TIPO) + "] " + String(__FILE__) + " [" + String(__LINE__) + "]: " + MESSAGE + "\n"; \
            if ( wifi_client && wifi_client.connected() ) wifi_client.println( log_message );\
            xSemaphoreGive(wifi_debugger_Mutex); \
        }
    #endif

    #ifdef DEBUG_REDUZIDO
    #define DEBUG_WIFI( TIPO, MESSAGE ) \
        if (xSemaphoreTake(wifi_debugger_Mutex, ( TickType_t ) pdMS_TO_TICKS(10) ) == pdTRUE) { \
        String log_message = String("[") + String(TIPO) + "]: " + MESSAGE + String("\r\n"); \
        if ( wifi_client && wifi_client.connected() ) wifi_client.println( log_message );\
        xSemaphoreGive(wifi_debugger_Mutex); \
        }
    #endif 
#endif


#ifdef DEBUG_DESLIGADO
#define DEBUG_WIFI( TIPO, MESSAGE ) {};
#endif 


/**
 * @brief Inicializa as interfaces WiFi para acesso do Telnet 
**/
void wifi_debugger_init( void );


/**
 * @brief Converte um buffer de bytes em uma representação de string hexadecimal.
 * @details Função itera sobre cada byte no buffer, converte-o para o seu equivalente
 * em string hexadecimal, e, anexa ao resultado da string. Para bytes menores que 0x10,
 * adiciona um zero à esquerda para manter um formato consistente de dois caracteres
 * para cada byte.
 * @note Essa função é util para ser utilizada em debugging de buffers: 
 *  ```wifi_client.print( "O buffer contem: " + buffer2string(buffer, len) )```
 * @param buffer Ponteiro para o buffer de bytes a ser convertido.
 * @param length O número de bytes no buffer a ser convertido.
 * @return Um objeto String contendo a representação hexadecimal dos bytes no buffer.
 */
String buffer2string( const uint8_t* buffer, size_t length);


#endif /* WIFI_DEBUGGER_H_ */