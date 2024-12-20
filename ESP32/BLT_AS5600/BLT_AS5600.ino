#include "BluetoothSerial.h"
#include <Wire.h>

#define ADDR_ID   0x36
#define ADDR_ANG  0x0C
#define LED_PIN   0x02 

BluetoothSerial SerialBT;

uint8_t getWire( uint8_t reg ) {
  Wire.beginTransmission( ADDR_ID );
  Wire.write( reg );
  Wire.endTransmission();
  Wire.requestFrom( ADDR_ID, 1);
  long int t0 = millis();
  while (Wire.available() == 0)
    if ( (millis() - t0) > 500 )
      return 0;
  return Wire.read();
}

void *read_raw_angle( uint8_t addr_reg, uint8_t num_regs ){
  digitalWrite( LED_PIN, false );
  uint8_t *data = (uint8_t *)malloc( sizeof(uint8_t) * num_regs );
  for ( uint8_t i = 0; i < num_regs; i++ ){
    data[i] = getWire( addr_reg + i );
  }
  digitalWrite( LED_PIN, false); 
  return data;
}

float read_angle( void ){
  uint16_t raw_angle = (uint16_t)read_raw_angle( ADDR_ANG, 2 );
  return (float)(raw_angle*360)/0xfff;
}

void setup() {
  // Inicia a Serial
  Serial.begin(115200);
  // Inicia o BLE
  SerialBT.begin("ESP32-BG");
  // Inicia o I2C do AS5600
  Wire.begin();
}

void loop() {
  while (SerialBT.available() > 0 ) {
    uint8_t read_ble = SerialBT.read();
    if (read_ble == 'R') {
      float angle = read_angle();
      SerialBT.write( angle );
      Serial.println( angle );
    }
  }
  delay(100);
}
