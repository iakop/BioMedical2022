#include <Bounce2.h> // Include the Bounce2 library, downloaded through: Sketch -> Include Library -> Manage Libraries, and searching for Bounce2

const int ledPin = 13; // ledPin is set to hold the "address" of pin 13
const int buttonPin = 12; // buttonPin is set to hold the "address" of pin 12

// Boolean values can be only either true or false:
bool ledState = false; // The variable ledState is initialized as a boolean false value
bool buttonState = false; // buttonState holds whether the button has been pushed 

// This is known as an object, different from a variable, that it has built-in functions
// They usually take care of complicated logic
Bounce button = Bounce(); // We create a Bounce object

void setup() {
  pinMode(ledPin, OUTPUT); // The LED's I/O pin is an output
  
  button.attach(buttonPin, INPUT); // The button's I/O pin is an input, for Bounce2 it's initialized through button.attach()
  button.interval(50); // Debounce interval is set
}

void loop() {

  button.update(); // Bounce button object checks the status of buttonPin

  // Check if the button rose (went from LOW to HIGH):
  buttonState = button.rose();
  if(buttonState == true){
    // The !-operator is used to represent the logical inverse (1 or 0, true or false), of a value
    ledState = !ledState; // We change ledState to the opposite of what is was
    digitalWrite(ledPin, ledState); //ledPin's output value is set to ledState's value (booleans also work as HIGH or LOW)
  }
  
}
