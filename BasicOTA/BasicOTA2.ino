#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <WiFiUDP.h>
#include <WiFi.h>
#include <TelnetStream.h>


#ifndef STASSID
#define STASSID "Sampaio"
#define STAPSK  "Elefante321"
#endif

// IP 192.168.18.147 PORT 3232 

void setup() {

  pinMode( 2, OUTPUT );
  
  Serial.begin(115200);
  Serial.println("Booting");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  
  ArduinoOTA.setPort(8080);
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  
  }).onEnd([]() {
    Serial.println("\nEnd");
  
  }).onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  
  }).onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)          Serial.println( "Auth Failed"     );
    else if (error == OTA_BEGIN_ERROR)    Serial.println( "Begin Failed"    );
    else if (error == OTA_CONNECT_ERROR)  Serial.println( "Connect Failed"  );
    else if (error == OTA_RECEIVE_ERROR)  Serial.println( "Receive Failed"  );
    else if (error == OTA_END_ERROR)      Serial.println( "End Failed"      );
  });

  ArduinoOTA.begin();
  TelnetStream.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

bool ledState = false; 

void loop() {
  ArduinoOTA.handle();

  if (ledState == true ){
    digitalWrite( 2, LOW );
    ledState = false;
  }else{
    digitalWrite( 2, HIGH );
    ledState = true;
  }
  
  delay(1000);
}
