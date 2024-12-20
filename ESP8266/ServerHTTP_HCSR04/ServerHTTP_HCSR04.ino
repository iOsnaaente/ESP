#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

#define TRIG_PIN 0
#define ECHO_PIN 2

const char* ssid = "Sampaio";
const char* psd  = "12051999";

IPAddress staticIP( 192, 168,  12, 125 );
IPAddress gateway ( 192, 168,  12,   1 );
IPAddress subnet  ( 255, 255, 255,   0 );

// Duration used to calculate distance
uint32_t duration;
uint32_t distance; 

// Criação do servidor na porta 80
ESP8266WebServer server(80);  


// Função para responder a requisições HTTP com o valor da distância
void handle_HTML_callback() {
  // Mede a distancia usando o HC-SR04 
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  
  digitalWrite(TRIG_PIN, LOW);
  
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration / 58.2;

  // Cria a resposta HTML 
  String html = "<html><body>";
  html += "<h1>Distância: " + String(distance) + " cm</h1>";
  html += "</body></html>";
  
  // Manda a resposta HTTP com o valor da distância
  server.send(200, "text/html", html);  
}


void setup() {
  Serial.begin(115200);

  Serial.printf("Connecting to %s\n", ssid);
  WiFi.config(staticIP, gateway, subnet);
  WiFi.begin(ssid, psd);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Define a função que será chamada quando acessar a raiz
  server.on("/", handle_HTTP_callback);  
  
  server.begin();  // Inicia o servidor web
  Serial.println("Server started.");
}


void loop() {
  server.handleClient();  // Lida com as requisições recebidas
}
