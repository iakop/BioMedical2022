const int ledPin = 13; // Pin 13 has a debug LED, so we specify a constant variable to hold this value

void setup() {
  // The code written inside setup()'s brackets is only run once:

  pinMode(ledPin, OUTPUT); // The I/O pin (13, as specified by the ledPin variable) is set to the mode OUTPUT

}

void loop() {
  // The code written inside loop()'s brackets is run forever, looping at the end:
  
  digitalWrite(ledPin, HIGH); // The ledPin I/O pin is set to HIGH/1/true (on)
  delay(1000); // delay() waits for the amount of milliseconds, passed as argument
  digitalWrite(ledPin, LOW); // The ledPin I/O pin is set to LOW/0/false (off)
  delay(1000); // delay() waits for another 1000ms
  
  // This is where the code loops
}
