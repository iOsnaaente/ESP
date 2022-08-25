#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> 


void setup() {
  WiFiManager wifiManager; 
  wifiManager.autoConnect( "ESP_BR-NO" );
}

void loop() {
  
}
