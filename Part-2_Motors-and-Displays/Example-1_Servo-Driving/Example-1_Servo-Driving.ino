#include <Servo.h> // We include the built-in Servo.h library

#define SERVOPIN 9 // SERVOPIN is 9
#define AIN A0 // Analog input is A0

// This is known as an object, different from a variable, that it has built-in functions
// They usually take care of complicated logic
Servo myServo; // We declare a Servo object, myServo

int potRead = 0; // potRead is initialized as 0.
int servoPos = 0; // servoPos, the servo position is initialized as 0.

unsigned long time = 0; // We create a time variable to store time since boot (obtained with millis()) 
unsigned long prevTime = 0; // prevTime is to compare elapsed time, to wait between prints, without using delay()
// millis() itself overflows after 4294967295ms/1000/60/60/24 = 49.71 days, meaning it restarts from 0
// Due to using unsigned long, the overflow causes no issue, it might otherwise

void setup() {
  myServo.attach(SERVOPIN); // myServo.attach() is an object function, setting up the pin SERVOPIN to drive the servo
  Serial.begin(9600); // We start the UART interface at 9600 baud
}

void loop() {
  potRead = analogRead(AIN); // Potentiometer is read on A0
  servoPos = map(potRead, 0, 1023, 5, 175); // potRead is mapped to a valid servoPos. The 9g servo travels between 0-180 deg. 5-175 is a safety precaution
  myServo.write(servoPos); // servoPos is passed to myServo.write() to set the servo

  // Sample the current millis(), and check if enough time elapsed:
  time = millis();
  if((time - prevTime) >= 1000){
    Serial.print("potRead = "); Serial.println(potRead); // potRead is written over Serial
    Serial.print("servoPos = "); Serial.println(servoPos); // servoPos is written over Serial
    prevTime = time; // prevTime is updated
  }
  
}