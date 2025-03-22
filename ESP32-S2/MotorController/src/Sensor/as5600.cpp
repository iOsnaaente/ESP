/**
  * @file As5600.cpp
  * @brief Classe de uso do sensor AS5600 
  * @author Bruno Gabriel Flores Sampaio, 
  * @date 23 de julho de 2024
  */

#include "as5600.h"


TwoWire as5600_Wire = TwoWire( SENSOR_AS5600_NUM );

void init_AS5600_Task(void *pvParameters) {
    as5600_Wire.begin( POS_SDA_PIN, POS_SCL_PIN, 100000UL );
    vTaskDelete(NULL);
}


void set_scan_period_ms( int32_t period ){
    period_to_scan = period;
}



int32_t read_raw(void) {
    as5600_Wire.beginTransmission(AS5600_ADDR);
    as5600_Wire.write( AS5600_ADDR_RAW_ANGLE );  
    as5600_Wire.endTransmission();
    as5600_Wire.requestFrom(AS5600_ADDR, 2);
    if (as5600_Wire.available() == 2) {
        uint16_t raw_angle = (as5600_Wire.read() << 8) | as5600_Wire.read();
        return raw_angle & 0x0FFF;  
    }
    return -1;
}


double read_scaled(void) {
    return (((double)read_raw() / AS5600_RESOLUTION) * 360.0);
}


uint8_t get_status(void) {
    uint8_t magnetic_status;
    esp_err_t ret = read_register_data(AS5600_ADDR_STATUS, 1, &magnetic_status);
    if (ret == ESP_OK) {
        if ((magnetic_status >> 5) & 0x01)      return  AS5600_MD_STATUS;
        else if ((magnetic_status >> 4) & 0x01) return  AS5600_ML_STATUS;
        else if ((magnetic_status >> 3) & 0x01) return  AS5600_MH_STATUS;
        else return AS5600_ERROR_STATUS;
    }else{
        return AS5600_ERROR_STATUS;
    }
}