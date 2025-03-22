#include "../boardConfig.h"

#include "Debugger/WiFiDebugger.h"
#include "Display/display_oled.h"
#include "Sensor/as5600.h"

int64_t actual_time = esp_timer_get_time();
int64_t last_time = actual_time;

int64_t period_to_scan = 10;

float last_position = 0.0; 
float last_velocity = 0.0; 
float velocity = 0.0; 
float position = 0.0; 

int32_t count = 0;

void setup() {

    // Inicializa o Debug via WiFi 
    // Selecionar o tipo de Debug: HTTP ou TelNet
    // boardConfig.h 
    wifi_debugger_init();
    DEBUG_WIFI( "INIT", "Wi-Fi Debugger inicializado!" );
    
    // Inicializa o Display I2C 
    if ( xTaskCreate( display_Task, "display_Task", 1024*8, NULL, 2, NULL ) == pdPASS ){
        DEBUG_WIFI( "SUCCESS", "Display Task inicializado com sucesso!" );
    } else {
        DEBUG_WIFI( "FAIL", "Display Task NÃO foi inicializado corretamente!" );
    }

    // Inicializa o Sensor AS5600 I2C 
    if ( xTaskCreate( init_AS5600_Task, "AS5600_Task", 1024*8, NULL, 2, NULL ) == pdPASS ){
        DEBUG_WIFI( "SUCCESS", "Sensor de Posição AS5600 Task inicializado com sucesso!" );
    } else {
        DEBUG_WIFI( "FAIL", "Sensor de Posição AS5600 Task NÃO foi inicializado corretamente!" );
    }

}


void loop() {

    last_position = position;
    position = read_raw();
    actual_time = esp_timer_get_time();
    velocity = (last_position - position) / (actual_time - last_time)*1000;
    last_velocity = velocity;
    last_time = actual_time;

    if ( count++ > 10 ){
        count = 0;
        DEBUG_WIFI( "POS", position );
        DEBUG_WIFI( "VEL", velocity );
        draw_screen( velocity, position );
    }
    
    vTaskDelay( pdMS_TO_TICKS(period_to_scan) );
}
