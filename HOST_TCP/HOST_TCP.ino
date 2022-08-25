#include <ESP8266WiFi.h>

WiFiServer server( 1205 ); // Porta de conexão
WiFiClient client;

#define ssid            "ESP32-Bruno" // Nome da conexão
#define password        ""            // Senha ou NULL para liberado
#define channel         2             // Canal utilizado default = 1 [1, 13]
#define hidden          false         // Rede oculta   
#define max_connection  8             // Máximo de conexões 8 

  
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP); // access point
  WiFi.softAP( ssid, password, channel, hidden, max_connection);
  server.begin();
}


void loop() {
  //Detecta se há clientes conectados no servidor.
  if (client.connected()) {
    //Verifica se o cliente conectado tem dados para serem lidos.
    if (client.available() > 0) {

      String req = "";
      while (client.available() > 0) {
        char z = client.read();
        req += z;
      }

      //Mostra a mensagem recebida do cliente no Serial Monitor.
      Serial.print("\nUm cliente enviou uma mensagem");
      Serial.print("\n...IP do cliente: ");
      Serial.print(client.remoteIP());
      Serial.print("\n...IP do servidor: ");
      Serial.print(WiFi.softAPIP());
      Serial.print("\n...Mensagem do cliente: " + req + "\n");

      //Envia uma resposta para o cliente
      client.print("\nO servidor recebeu sua mensagem");
      client.print("\n...Seu IP: ");
      client.print(client.remoteIP());
      client.print("\n...IP do Servidor: ");
      client.print(WiFi.softAPIP());
      client.print("\n...Sua mensagem: " + req + "\n");
    }
  }
  
  //Se nao houver cliente conectado,
  else{
    //Disponabiliza o servidor para o cliente se conectar.
    client = server.available();
    delay(1);
  }
}
