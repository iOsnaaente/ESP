/** @brief This example shows how to take simple range 
  measurements with the VL53L1X. The range readings are
  in units of mm.
*/

#include <Arduino.h>
#include <Wire.h>

#include <VL53L1X.h>

VL53L1X sensor;

void setup(){ 

  Serial.begin(921600, SERIAL_8N1 );
  while (!Serial) { }
  
  Wire.begin();
  Wire.setClock(400000); 

  sensor.setTimeout(100);
  if (!sensor.init()){
    Serial.println("Failed to detect and initialize sensor!");
    while (true);
  }

  // Use long distance mode and allow up to 50000 us (50 ms) for a measurement.
  // You can change these settings to adjust the performance of the sensor, but
  // the minimum timing budget is 20 ms for short distance mode and 33 ms for
  // medium and long distance modes. See the VL53L1X datasheet for more
  // information on range and timing limits.
  sensor.setDistanceMode(VL53L1X::Short);
  sensor.setMeasurementTimingBudget(20000);

  // Start continuous readings at a rate of one measurement every 50 ms (the
  // inter-measurement period). This period should be at least as long as the
  // timing budget.
  sensor.startContinuous(50);
}

void loop(){
  Serial.printf("Sensor read: %d\n" , sensor.read() );
  Serial.printf("Range: %d\n"       , sensor.ranging_data.range_mm );
  Serial.printf("Status: %s\n"      , VL53L1X::rangeStatusToString(sensor.ranging_data.range_status));
  Serial.printf("Peak signal: %f\n" , sensor.ranging_data.peak_signal_count_rate_MCPS );
  Serial.printf("Ambient: %f\n"     , sensor.ranging_data.ambient_count_rate_MCPS );
}
