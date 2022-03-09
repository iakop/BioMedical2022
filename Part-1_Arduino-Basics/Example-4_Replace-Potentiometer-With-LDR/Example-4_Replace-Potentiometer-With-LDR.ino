#define LEDPIN 6 // We #define, another way to set a constant value LEDPIN is 6 here
#define LDRPIN A0 // LDRPIN is A0

int ldrVal = 0; // A variable integer is set up to hold the potentiometer value, initialized to 0
int pwmOut = 0; // A variable integer is set up to hold the PWM output value

void setup() {
  pinMode(LEDPIN, OUTPUT); // LEDPIN is output
  Serial.begin(9600); // We specify and begin serial communication (UART protocol) between Arduino and Computer at baudrate of 9600 baud (symbols/s)
}

void loop() {
  ldrVal = analogRead(LDRPIN); // We perform an analogRead() on ANALOGIN, store it in ldrVal
  Serial.print("ldrVal = "); // We print the name of ldrVal to UART
  Serial.println(ldrVal); // Then we print the value of ldrVal to UART Serial.println() also changes to next line after

  pwmOut = map(ldrVal,1023,344,0,255); // pwmOut gets the ldrVal, mapped inversely through map(), so a high voltage gives low light
  analogWrite(LEDPIN, pwmOut); // LEDPIN generates a PWM signel, the dutycycle resolution (0-100%) is given by 0-255
  Serial.print("Dutycycle = "); // Dutycycle is printed
  Serial.print((float)pwmOut/255.0*100.0); // We calculate dutycycle as the value printed
  Serial.println("%"); // And add a percentage sign and a linechange
}
