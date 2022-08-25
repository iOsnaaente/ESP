#include <ESP8266WiFi.h>

#define trigPin 0
#define echoPin 2

const char* ssid = "Robotica UFSM";
const char* password = "robotica2022";

IPAddress staticIP(192,168,1,125);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

void setup(void){
  Serial.begin(115200);

  Serial.printf("Connecting to %s\n", ssid);
  
  WiFi.config( staticIP, gateway, subnet );
  WiFi.begin( ssid, password );
  
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

long duration, distance; // Duration used to calculate distance

void loop(void) {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration / 58.2;
  
  Serial.println(distance);
  delay(200);
}
