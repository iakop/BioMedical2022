#define LEDPIN 6 // We #define, another way to set a constant value LEDPIN is 6 here
#define ANALOGIN A0 // ANALOGIN is A0

int potVal = 0; // A variable integer is set up to hold the potentiometer value, initialized to 0
int pwmOut = 0; // A variable integer is set up to hold the PWM output value

void setup() {
  pinMode(LEDPIN, OUTPUT); // LEDPIN is output
  Serial.begin(9600); // We specify and begin serial communication (UART protocol) between Arduino and Computer at baudrate of 9600 baud (symbols/s)
}

void loop() {
  potVal = analogRead(ANALOGIN); // We perform an analogRead() on ANALOGIN, store it in potVal
  Serial.print("potVal = "); // We print the name of potVal to UART
  Serial.println(potVal); // Then we print the value of potVal to UART Serial.println() also changes to next line after

  pwmOut = potVal/4; // pwmOut gets the value of potVal/4 1024(resolution of 10bit read)/4 different values gives 256, the resolution of analogWrite
  analogWrite(LEDPIN, pwmOut); // LEDPIN generates a PWM signel, the dutycycle resolution (0-100%) is given by 0-255
  Serial.print("Dutycycle = "); // Dutycycle is printed
  Serial.print((float)pwmOut/255.0*100.0); // We calculate dutycycle as the value printed
  Serial.println("%"); // And add a percentage sign and a linechange
}
