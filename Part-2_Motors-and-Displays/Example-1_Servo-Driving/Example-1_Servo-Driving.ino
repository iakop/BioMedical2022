#include <Servo.h> // Her inkluderes Servo biblioteket.

#define SERVPIN 9 // Servopinnen defineres til at v�re pin 9
#define AIN A0 // Pinnen til analog l�sning bliver defineret.

Servo myServo; // Et servo-objekt, myServo instantieres.
int potRead = 0; // potRead initialiseres til 0.
int servoPos = 0; // Servopositionen servoPos instantieres til 0.

long unsigned timer = 0;

void setup() {
  myServo.attach(SERVPIN); // myServo s�ttes til p� pin 9 (PWM pin).
  Serial.begin(9600); // Den serielle forbindelse p�begyndes.
}

void loop() {
  potRead = analogRead(AIN); // potentiometret afl�ses til potRead.
  servoPos = map(potRead, 0, 1023, 1, 179); // potRead omregnes til en servoPos.
  myServo.write(servoPos); // servoPos (i grader) skrives til myServo.

  if(timer <= millis() - 1000){
    Serial.print("potRead = "); Serial.println(potRead); // potReads v�rdi skrives ud p� den serielle forbindelse.
    Serial.print("servoPos = "); Serial.println(servoPos); // servoPos' v�rdi skrives ud p� den serielle forbindelse.
    timer = millis();
  }
  
}