#include <LiquidCrystal.h> // Vi indl�ser LiquidCrystal biblioteket.

#define RS 3
#define E 4
#define D4 5
#define D5 6
#define D6 7
#define D7 8
// RW til GND
// VSS til GND
// VDD til 5V
// 10k potmeter mellem GND VO og VDD
// A forbindes med en 220 ohms resistor til +5V og en K til GND.

#define BTN 2 // Knappen sidder p� pin 2.

//Vores Helt eget tegn:
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

LiquidCrystal lcd(RS, E, D4, D5, D6, D7); // Vi constructer et objekt lcd af typen Liquid Crystal.

unsigned int count = 0; // count er den t�llerv�rdi vi vil skrive ud i sidste ende.
long unsigned int wait = 0; // wait er en timervariabel, der hj�lper os med at holde styr p� ventetiden for knappen.

void setup() {
  pinMode(BTN, INPUT); // BTN s�ttes op som input.
  
  lcd.createChar(0, blinkey); // createChar funktionen bruges til at s�tte vores eget tegn op.
  
  lcd.begin(16, 2); // LCD'et ops�ttes med 2 linjer, 16 chars.
  
  lcd.setCursor(0 /*x*/ , 0 /*y*/); // Cursoren for displayet s�ttes p� koordinaterne 0,0.
  lcd.print("Hello world! "); // Vi udskriver Hello World.
  
  lcd.write(byte(0)); // Vi skriver vores eget tegn.
}

void loop() {
  // Vi beh�ver kun �ndre noget, hvis der trykkes p� knappen:
  if (digitalRead(BTN) == HIGH && wait <= (millis() - 100)) //Hvis knappen er h�j, og ventetimeren er 100 ms efter systemtimeren.
  {
    wait = millis(); // Ventetimeren opdateres.
    count++; // Count inkrementeres med 1.
    
    lcd.setCursor(0, 1); // S�t cursoren ned p� den rigtige plads.
    lcd.print(count); // Udskriv count.
    
    while(digitalRead(BTN) == HIGH); // TRICK! Programmet laver intet i en while-l�kke der bare er while(cond);
    delay(50); // N�r knappen g�r ned fra HIGH igen, vent 50ms.
  }
}