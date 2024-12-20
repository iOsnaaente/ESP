#include "Servo.h"

#define SERVO_GPIO  1
#define LED_GPIO    2 

Servo servo; 

void setup() {
  /* Define o pino do LED como saída */
  pinMode( LED_GPIO, OUTPUT);
  /* Instancia o Servo */
  servo.attach( SERVO_GPIO ); 
  /* Define o botão a ser pressionado */
  pinMode( LED_GPIO, INPUT_PULLUP);
}


void loop() {
  /* Verifica o estado do botão de entrada */
  if ( digitalRead(LED_GPIO) == 0 ){
    servo.write(100);

  }else{
    servo.write(0);
  }
  delay(100);
}