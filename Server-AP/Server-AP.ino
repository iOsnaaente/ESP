#include <WiFi.h>

const char* ap_ssid = "Robotica UFSM";  //Access Point SSID
const char* ap_password = "robotica2022";   //Access Point Password
uint8_t max_connections = 8;           //Maximum Connection Limit for AP
int current_stations = 0;
int new_stations = 0;

void setup(){
  //Start the serial communication channel
  Serial.begin(115200);
  Serial.println();

  //Setting the AP Mode with SSID, Password, and Max Connection Limit
  if (WiFi.softAP(ap_ssid, ap_password, 1, false, max_connections) == true){
    Serial.print("Access Point is Creadted with SSID: ");
    Serial.println(ap_ssid);
    
    Serial.print("Max Connections Allowed: ");
    Serial.println(max_connections);
   
    Serial.print("Access Point IP: ");
    Serial.println(WiFi.softAPIP());
  }
  else{
    Serial.println("Unable to Create Access Point");
  }
}

void loop(){
  //Continuously check how many stations are connected to Soft AP and notify whenever a new station is connected or disconnected
  new_stations = WiFi.softAPgetStationNum();

  if (current_stations < new_stations){  // Device connected 
    current_stations = new_stations;
    Serial.print("New Device Connected to SoftAP... Total Connections: ");
    Serial.println(current_stations);
  }

  if (current_stations > new_stations){ //Device is Disconnected
    current_stations = new_stations;
    Serial.print("Device disconnected from SoftAP... Total Connections: ");
    Serial.println(current_stations);
  }

}
