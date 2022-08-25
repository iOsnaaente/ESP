#include "BluetoothSerial.h"
#include <Wire.h>

BluetoothSerial SerialBT;

uint8_t ADDR_ID   = 0x36;
uint8_t ADDR_ANG  = 0x0C;

uint16_t raw_angle = 0;
float    angle = 0.0;

void setup() {
  digitalWrite( 4, LOW ); 
  
  // Inicia a Serial
  Serial.begin(115200);

  // Inicia o BLE
  SerialBT.begin("ESP32-BG");

  // Inicia o I2C do AS5600
  Wire.begin();
}


uint8_t getWire( byte reg ) {
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

void loop() {
  while (SerialBT.available() > 0 ) {
    byte read_ble = SerialBT.read();
    if (read_ble == 'R') {
      digitalWrite( 2, LOW );

      raw_angle  = ( getWire( 0xC )<<8);
      raw_angle +=   getWire( 0xD );
      angle = (float)(raw_angle*360)/0xfff ;

      SerialBT.write( angle );
      Serial.println( angle );

      digitalWrite( 2, LOW); 
    }
  }
}
