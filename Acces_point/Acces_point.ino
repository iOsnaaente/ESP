#include <WiFi.h>

const char* ssid = "SmartWatchBG";
const char* password = "12051999";
WiFiServer server(80);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  
  IPAddress staticIP(192, 168,  12,  80); // IP set to Static
  IPAddress gateway( 192, 168,  12,   1); // gateway set to Static
  IPAddress subnet(  255, 255, 255,   0); // subnet set to Static

  WiFi.mode(WIFI_AP);                     // Working mode only as Acess Point 

  WiFi.softAP(ssid, password, 2, 0);      // SSID PASSWORD CANAL (1-11) [REDE_NÂO_OCULTA(0) OCULTA(1)]
  WiFi.config(staticIP, gateway, subnet);

  server.begin();

  Serial.println("Server started"); 
  Serial.println(WiFi.softAPIP());
}

long int randomNumber = random(0, 99999); 

void loop() {
  
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  while (!client.available()) {
    delay(1);
    // \trava o código, sugiro usar um timmer ou contador de tempo (timeout)
  }

  String req = client.readStringUntil('\r');
  req = req.substring(req.indexOf("/") + 1, req.indexOf("HTTP") - 1);
  Serial.println(req);
  client.flush();

  // Match the request - Aqui escrevemos as funções 
  if (req.indexOf("D") != -1){
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();
    client.println(randomNumber);
    client.println();
  }
  else if (req.indexOf("R") != -1){
    randomNumber = random(0, 99999); 
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();
    client.println(randomNumber);
    client.println();
  }  
  else {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.print("Invalid Request");
    client.flush();
    client.stop();
    return;
  }
  client.flush();

}
