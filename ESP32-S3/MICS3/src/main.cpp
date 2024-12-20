#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "driver/gpio.h"
#include "driver/i2s.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"


// Connections to INMP441 I2S microphone
#define I2S_WS  GPIO_NUM_17
#define I2S_SD  GPIO_NUM_16
#define I2S_SCK GPIO_NUM_15
#define I2S_LR  false 

// Use I2S Processor 0
#define I2S_PORT I2S_NUM_0

// Define input buffer length
#define I2S_BUFFER_LEN 512

// Buffer to store I2S data read from the microphone
int8_t i2s_buffer[ I2S_BUFFER_LEN*sizeof(int16_t) ];

void i2s_Task( void *pvParameters ){
  // Inicialize I2S 
  i2s_config_t i2s_config;
  i2s_config.mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX); // MIC is a Receiver RX 
  i2s_config.sample_rate = 44100;                              // 44.1KHz 
  i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;      // 32 bits per sample
  i2s_config.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;       // Only Left Channel
  i2s_config.communication_format = I2S_COMM_FORMAT_STAND_I2S; // I2S standard
  i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;          // Without Interrupt Level
  i2s_config.dma_buf_count = 16;
  i2s_config.dma_buf_len = I2S_BUFFER_LEN/sizeof(int16_t);
  i2s_config.use_apll = false;    

  // I2S Pin configurations 
  i2s_pin_config_t pin_config;
  pin_config.bck_io_num = I2S_SCK;
  pin_config.ws_io_num = I2S_WS;
  pin_config.data_out_num = I2S_PIN_NO_CHANGE;
  pin_config.data_in_num = I2S_SD;
  
  // Use install I2S to initialize the driver 
  i2s_driver_install( I2S_PORT, &i2s_config, 0, NULL );
  
  // Set the pin configuration 
  i2s_set_pin( I2S_PORT, &pin_config );

  // Inicialize the I2S 
  i2s_start( I2S_PORT );
  

  // Variables to store the number of bytes read by the I2S buffer
  size_t bytes_read_by_i2s_buffer;
  
  // Variable to store the I2S errors 
  int ret;

  // RTOS Task Loop to read continuously the I2S data 
  while ( true ){

    // Read the I2S data 
    esp_err_t result = i2s_read(I2S_PORT, i2s_buffer, I2S_BUFFER_LEN*sizeof(int16_t), &bytes_read_by_i2s_buffer, portMAX_DELAY);
    if (result == ESP_OK && bytes_read_by_i2s_buffer > 0) {
      Serial.write( (uint8_t *) &i2s_buffer, bytes_read_by_i2s_buffer ); 
    }
    
  }

}


void led_blink_Task( void *pvParameters ){
  // Initialize the NeoPixel RGB Led builtin in Board 
  neoPixelType led_type = NEO_GRB + NEO_KHZ800;
  Adafruit_NeoPixel board_led = Adafruit_NeoPixel( 1, GPIO_NUM_48, led_type );
  
  // Configure the RGB Led ouput 
  gpio_set_direction( GPIO_NUM_48, GPIO_MODE_OUTPUT );
  board_led.begin(); 
  board_led.clear();

  // Set the RGB Led color to off 
  board_led.setPixelColor( 0, board_led.Color(0, 0, 0));
  board_led.show();
  
  while ( true ){
    // Do nothing
    vTaskDelay( portMAX_DELAY );
  }

  vTaskDelete(NULL);
}



void setup() {
  Serial.begin(1000000);
  xTaskCreate( i2s_Task, "i2s_Task", 40960, NULL, 1, NULL );
  xTaskCreate( led_blink_Task, "blink_Task", 4096, NULL, 1, NULL );
}


void loop() {
  vTaskDelay( portMAX_DELAY );
}

