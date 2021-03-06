// TFT libraries:
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// MAX30102 libraries:
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

// SPI Signals
#define SCK 13
#define MOSI 11
#define MISO 12
#define DC 8
#define CS 10

// Controls for reset and backlight
#define RST 9
#define BL 6

#define MAXBUF 50
#define BUFWINDOW 10

// Defines a tft object:
Adafruit_ILI9341 tft = Adafruit_ILI9341(CS, DC, RST);

// Pulse oximeter:
MAX30105 pulseOximeter;

const byte RATE_SIZE = 4; // Increase this for more averaging 4 is good
byte rates[RATE_SIZE]; // Array of heart rate measurements to average over
byte rateSpot = 0;
long unsigned lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;

void setup() {
  Serial.begin(115200);
  tft.begin(); // Start the display

  if (!pulseOximeter.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println(F("MAX30102 was not found. Please check wiring/power."));
    while (1);
  }

  pulseOximeter.setup(); //Configure sensor with default settings
  pulseOximeter.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  pulseOximeter.setPulseAmplitudeGreen(0); //Turn off Green LED
  
  pinMode(BL, OUTPUT); // Set up the backlight
  digitalWrite(BL, HIGH); // Turn it on
  
  drawGraph(); // Draw the initial graph
  
}

void loop() {
  static long irVal;

  // Update the irVal until there is a new beat:
  do{
    irVal = pulseOximeter.getIR();
  }
  while(!checkForBeat(irVal));
  
  //We sensed a beat!
  long unsigned delta = millis() - lastBeat;
  lastBeat = millis();

  beatsPerMinute = 60 / (delta / 1000.0);

  // Sanity check the beats:
  if (beatsPerMinute < 255 && beatsPerMinute > 20){
    rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
    rateSpot %= RATE_SIZE; //Wrap variable

    //Take average of readings
    beatAvg = 0;
    for (int x = 0 ; x < RATE_SIZE ; x++)
      beatAvg += rates[x];
    beatAvg /= RATE_SIZE;
  }

  Serial.print("IR=");
  Serial.print(irVal);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.println(beatAvg);
  plotVal(beatAvg);
  
}

void drawGraph() {
  tft.fillScreen(ILI9341_BLACK); // Black-out the screen
  tft.setRotation(3); // We want it horizontal

  // All the text is white:
  tft.setTextColor(ILI9341_WHITE);

  // Place and draw the title:
  tft.setCursor(118, 0);
  tft.setTextSize(2);
  tft.println("My Pulse");

  // Draw a "0" by the origin:
  tft.setCursor(10, 220); tft.setTextSize(1); tft.println("0");
  tft.drawLine(20, 220, 300, 220, ILI9341_WHITE);
  tft.drawLine(20, 220, 20, 20, ILI9341_WHITE);

  // Draw the x and y labels:
  char xlabel[] = "Time";
  char ylabel[] = "Pulse BPM";
  tft.setRotation(2); // Set rotation to draw sideways
  tft.setCursor(100, 8); tft.setTextSize(1); tft.println(ylabel);
  tft.setRotation(3); // Set the rotation back
  tft.setCursor(150, 225); tft.setTextSize(1); tft.println(xlabel);
  
}

void plotVal (int val) {
  const int MAX_X = 278;
  static int prevX, curX = 0; // X values ranging from 0 to MAX_X
  static int prevY, curY = 0; // Y values ranging from 0 to 1023 (analogRead max)

  curX += 10; // Increment by 1 whenever a new plot is called.
  curY = val; // For this function, curY is completely the same as val.

  // If plot exceeds boundaries, it should be drawn from the beginning:
  if(curX >= MAX_X){
    // Reset current and previous x.
    prevX = 0;
    curX = 0; 
    blankGraph();
  }

  // Map all the values to fit within plot boundaries:
  int plotX1 = map(prevX, 0, 278, 21, 299);
  int plotY1 = map(prevY, 0, 255, 219, 21);
  int plotX2 = map(curX, 0, 278, 21, 299);
  int plotY2 = map(curY, 0, 255, 219, 21);

  //Draw a line with the mapped values:
  tft.drawLine(plotX1, plotY1, plotX2, plotY2, ILI9341_CYAN);

  // Set the new prev values:
  prevX = curX;
  prevY = curY;
}

void blankGraph(){
  for (int x = 0; x <= 278; x++){
    tft.drawFastVLine(x+21, 20, 200, ILI9341_BLACK);
  }
}
