/**
 * @file display_oled.h 
 * @brief Biblioteca de uso para o Display  
 * @author Bruno Gabriel Flores Sampaio,
 * @date 14 de março de 2025
*/

#ifndef DISPLAY_OLED_H_
#define DISPLAY_OLED_H_

#include "../boardConfig.h"

#include "Debugger/WiFiDebugger.h"

#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>


/* Display OLED */
#define SCREEN_WIDTH            128
#define SCREEN_HEIGHT           64
#define OLED_RESET              -1

#define DISPLAY_OLED_ADDRESS    0x3C 

extern Adafruit_SSD1306 *display;
extern TwoWire display_Wire;

extern float to_draw_position;
extern float to_draw_speed;
extern bool display_redraw;


void display_Task( void *pvParameters );
void reset_display( void );
void draw_speed( float speed );
void draw_position( float count );
void draw_screen( float speed, float position );

#endif /* DISPLAY_OLED_H_ */