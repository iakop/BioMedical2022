const int ledPin = 13; // ledPin is set to hold the "address" of pin 13
const int buttonPin = 12; // buttonPin is set to hold the "address" of pin 12

// Boolean values can be only either true or false:
bool ledState = false; // The variable ledState is initialized as a boolean false value

void setup() {
  pinMode(ledPin, OUTPUT); // The LED's I/O pin is an output
  pinMode(buttonPin, INPUT); // The button's I/O pin is an input
}

void loop() {
  if (digitalRead(buttonPin) == HIGH){ // If buttonPin's value is equal to HIGH/1/true/ (on):
    
    // The !-operator is used to represent the logical inverse (1 or 0, true or false), of a value
    ledState = !ledState; // We change ledState to the opposite of what is was
    digitalWrite(ledPin, ledState); //ledPin's output value is set to ledState's value (booleans also work as HIGH or LOW)
    
    // delay() waits for the amount of milliseconds, passed as argument:
    delay(200); // 200 milliseconds delay ensures that mechanical bounce is eliminated
  }
}
