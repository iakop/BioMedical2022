#include <LiquidCrystal.h> // Include the built-in LiquidCrystal.h library
#include <Bounce2.h> // To debounce button presses

// Pin defines
#define RS 3
#define E 4
#define D4 5
#define D5 6
#define D6 7
#define D7 8
// RW -> GND
// VSS -> GND
// VDD -> 5V
// 10k potentiometer between GND VO and VDD
// A through 220ohm resistor to +5V
// K -> GND

#define BTN 2 // BTN is on pin 2
bool buttonState = false;

// We make a byte array. This becomes our custom character on the display:
byte blinkey[8] =
{
  B00000,
  B11001,
  B00000,
  B00000,
  B11111,
  B01001,
  B00110
};

LiquidCrystal lcd(RS, E, D4, D5, D6, D7); // We instantiate a LiquidCrystal object, lcd
Bounce button = Bounce(); // We create a Bounce object

unsigned int count = 0; // Count is how many times we pushed the button

void setup() {
  button.attach(BTN, INPUT); // The button's I/O pin is an input, for Bounce2 it's initialized through button.attach()
  button.interval(50); // Set debounce interval
  
  lcd.createChar(0, blinkey); // createChar funktionen bruges til at sætte vores eget tegn op.
  
  lcd.begin(16, 2); // LCD'et opsættes med 2 linjer, 16 chars.
  
  lcd.setCursor(0 /*x*/ , 0 /*y*/); // Cursoren for displayet sættes på koordinaterne 0,0.
  lcd.print("Hello world! "); // Vi udskriver Hello World.
  
  lcd.write(byte(0)); // Vi skriver vores eget tegn.
}

void loop() {
  
  // Check for changes on button:
  button.update();
  buttonState = button.rose();

  // We only need to update the display when button is pushed:
  if(buttonState == true) { 
    count++; // count is incremented by 1
    
    lcd.setCursor(0, 1); // We place the cursor
    lcd.print(count); // print count
  }
}
