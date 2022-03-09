#include <Wire.h>
#include "MAX30105.h"
// TFT libraries:
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
// SPI Signals
#define SCK 13
#define MOSI 11
#define MISO 12
#define DC 8
#define CS 10

// Controls for reset and backlight
#define RST 9
#define BL 6

#define AVGWINDOW 64 // Size of averaging window
#define MINMAXROOM 400 // The values plotted max/min out at +-200

// Defines a tft object:
Adafruit_ILI9341 tft = Adafruit_ILI9341(CS, DC, RST);

MAX30105 pulseOximeter;
long avgVal = 0;
long minVal = 0;
long maxVal = 0;

void setup()
{
  tft.begin(); // Start the display
  Serial.begin(115200);
  Serial.println("Initializing...");

  // Initialize sensor
  if (!pulseOximeter.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30102 was not found. Please check wiring/power.");
    while (1);
  }

  pinMode(BL, OUTPUT); // Set up the backlight
  digitalWrite(BL, HIGH); // Turn it on
  
  drawGraph(); // Draw the initial graph

  // Setup to sense a nice looking saw tooth graph
  byte ledBrightness = 0x1F; // Options: 0=Off to 255=50mA
  byte sampleAverage = 8; // Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 3; // Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  int sampleRate = 100; // Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; // Options: 69, 118, 215, 411
  int adcRange = 4096; // Options: 2048, 4096, 8192, 16384

  pulseOximeter.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); // Configure sensor with these settings

  //Arduino plotter auto-scales annoyingly. To get around this, pre-populate
  //the plotter with 500 of an average reading from the sensor

  //Take an average of IR readings at power up
  
  for (int x = 0 ; x < AVGWINDOW ; x++)
  {
    avgVal += pulseOximeter.getIR(); //Read the IR value
  }
  avgVal /= AVGWINDOW;
  minVal = avgVal-MINMAXROOM;
  maxVal = avgVal+MINMAXROOM;
}

void loop()
{
  plotVal(pulseOximeter.getIR()); //Send raw data to plotter
}

void drawGraph() {
  tft.fillScreen(ILI9341_BLACK); // Black-out the screen
  tft.setRotation(3); // We want it horizontal

  // All the text is white:
  tft.setTextColor(ILI9341_WHITE);

  // Place and draw the title:
  tft.setCursor(118, 0);
  tft.setTextSize(2);
  tft.println("My Graph");

  // Draw a "0" by the origin:
  tft.setCursor(10, 220); tft.setTextSize(1); tft.println("0");
  tft.drawLine(20, 220, 300, 220, ILI9341_WHITE);
  tft.drawLine(20, 220, 20, 20, ILI9341_WHITE);

  // Draw the x and y labels:
  char xlabel[] = "Time";
  char ylabel[] = "SensorVal";
  tft.setRotation(2); // Set rotation to draw sideways
  tft.setCursor(100, 8); tft.setTextSize(1); tft.println(ylabel);
  tft.setRotation(3); // Set the rotation back
  tft.setCursor(150, 225); tft.setTextSize(1); tft.println(xlabel);
  
}

void plotVal (long val) {
  const long MAX_X = 278;
  static long prevX, curX = 0; // X values ranging from 0 to MAX_X
  static long prevY, curY = avgVal; // Y values ranging from 0 to 1023 (analogRead max)

  curX++; // Increment by 1 whenever a new plot is called.
  curY = val; // For this function, curY is completely the same as val.

  if (curY > maxVal){
    curY = maxVal;
  }
  else if(curY < minVal){
    curY = minVal;
  }

  // If plot exceeds boundaries, it should be drawn from the beginning:
  if(curX >= MAX_X){
    // Reset current and previous x.
    prevX = 0;
    curX = 0; 
    blankGraph();
  }

  // Map all the values to fit within plot boundaries:
  int plotX1 = map(prevX, 0, 278, 21, 299);
  int plotY1 = map(prevY, minVal, maxVal, 219, 21);
  int plotX2 = map(curX, 0, 278, 21, 299);
  int plotY2 = map(curY, minVal, maxVal, 219, 21);

  Serial.println(curY);

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
