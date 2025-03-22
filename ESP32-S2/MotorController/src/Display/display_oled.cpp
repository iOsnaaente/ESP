/**
 * @file display_oled.cpp 
 * @brief Biblioteca de uso para o Display  
 * @author Bruno Gabriel Flores Sampaio,
 * @date 14 de março de 2025
*/

#include "display_oled.h"

TwoWire display_Wire = TwoWire( DISPLAY_OLED_NUM );

Adafruit_SSD1306 *display;

float to_draw_position = 0.0;
float to_draw_speed = 0.0;
bool display_redraw = false;


void display_Task( void *pvParameters ){
    (void) pvParameters;
    
    // Inicializa o objeto do OLED 
    display_Wire.begin( DISPLAY_SDA_PIN, DISPLAY_SCL_PIN, 1000000UL );
    display = new Adafruit_SSD1306( SCREEN_WIDTH, SCREEN_HEIGHT, &display_Wire, OLED_RESET );
    if ( !display->begin( SSD1306_SWITCHCAPVCC, DISPLAY_OLED_ADDRESS ) ) {
        DEBUG_WIFI( "OLED FAIL", "Falha ao iniciar o display OLED");
    }
    // Reinicia o Display para começar a desenhar 
    reset_display();
    // while( true ){
    //     draw_speed( to_draw_speed );
    //     draw_turns( to_draw_position );
    //     if ( display_redraw ){
    //         display_redraw = !display_redraw;
    //         reset_display();
    //     }
    //     vTaskDelay( pdMS_TO_TICKS(1000) );
    // }
    vTaskDelete(NULL);
}


void reset_display( void ){
    // Printa a inicialização 
    display->clearDisplay();
    display->setCursor(5, 0);
    display->setTextSize(2);
    display->setTextColor(SSD1306_WHITE);
    display->print( "Controle:" );
    display->display();

    to_draw_position = 0.0;
    to_draw_speed = 0.0;

    // Zera as variáveis 
    draw_speed( to_draw_speed );
    draw_position( to_draw_position );
}

void draw_speed( float speed ) {
    display->setCursor(5, 25);
    display->setTextSize(2);
    display->printf("Vel: %.1f \t", speed );
    display->display();
}

void draw_position( float pos ) {
    display->setCursor(5, 45);
    display->setTextSize(2);
    display->printf("Pos: %.1f \t", pos );
    display->display();
}

void draw_screen( float speed, float position ){
    display->clearDisplay();
    display->setCursor(5, 0);
    display->setTextSize(2);
    display->setTextColor(SSD1306_WHITE);
    display->print( "Controle:" );
    display->setCursor(5, 25);
    display->setTextSize(2);
    display->printf("Vel: %.1f \t", speed );
    display->setCursor(5, 45);
    display->setTextSize(2);
    display->printf("Pos: %.1f \t", position );
    display->display();
}
