// TFT libraries:
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// MAX30102 libraries:
#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

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
// Buffers for sensor data:
uint16_t irBuf[MAXBUF];
uint16_t redBuf[MAXBUF];

int32_t bufLen = MAXBUF; //data buffer length
int32_t spo2; //SpO2 value
int8_t validSpo2; //indicator to show if the SPO2 calculation is valid
int32_t heartRate; //heart rate value
int8_t validHeartRate; //indicator to show if the heart rate calculation is valid

// Set the MAX30102 settings:
const byte ledBrightness = 60; // Options: 0=Off to 255=50mA
const byte sampleAverage = 4; // Options: 1, 2, 4, 8, 16, 32
const byte ledMode = 3; // Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
const byte sampleRate = MAXBUF*2; // Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
const int pulseWidth = 411; // Options: 69, 118, 215, 411
const int adcRange = 4096; // Options: 2048, 4096, 8192, 16384

void setup() {
  Serial.begin(115200);
  tft.begin(); // Start the display

  /*
  int result = pulseOximeter.begin(Wire, I2C_SPEED_FAST); // Use default SDA/A4 SCL/A5 at 400kHz speed
  // If the setup wasn't succesful
  if(result != 0){
    Serial.println(F("MAX30102 was not found. Attach and then reset"));
    while (1); // Stay here until reset
  } */

  if (!pulseOximeter.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println(F("MAX30105 was not found. Please check wiring/power."));
    while (1);
  }
  
  // Passing the settings:
  pulseOximeter.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
  
  pinMode(BL, OUTPUT); // Set up the backlight
  digitalWrite(BL, HIGH); // Turn it on
  
  drawGraph(); // Draw the initial graph
  
  initSamples(); // Calculate SpO2 and heart rate on the first MAXBUF samples
}

void loop() {
  static long prevSpo2 = 0;

  updateSamples();

  if (validSpo2 == 0){
    plotVal(prevSpo2);
  }
  else{
    plotVal(spo2);
    prevSpo2 = spo2;
  }
  
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

void plotVal (int val) {
  const int MAX_X = 278;
  static int prevX, curX = 0; // X values ranging from 0 to MAX_X
  static int prevY, curY = 0; // Y values ranging from 0 to 1023 (analogRead max)

  curX+= 2; // Increment by 1 whenever a new plot is called.
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
  int plotY1 = map(prevY, 0, 100, 219, 21);
  int plotX2 = map(curX, 0, 278, 21, 299);
  int plotY2 = map(curY, 0, 100, 219, 21);

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

void initSamples(){
  for (int i = 0; i < bufLen; i++)
  {
    // Wait until new data is available
    while (pulseOximeter.available() == false){
      pulseOximeter.check(); // Ask the sensor to check
    }
    
    // Get the samples:
    redBuf[i] = pulseOximeter.getRed();
    irBuf[i] = pulseOximeter.getIR();
    pulseOximeter.nextSample(); // Prepare next sample

    Serial.print(F("red="));
    Serial.print(redBuf[i], DEC);
    Serial.print(F(", ir="));
    Serial.println(irBuf[i], DEC);
  }

  // Run the algorithm for SpO2 calculation
  maxim_heart_rate_and_oxygen_saturation(irBuf, bufLen, redBuf, &spo2, &validSpo2, &heartRate, &validHeartRate);
}

void updateSamples(){
  // Rotate out the oldest BUFWINDOW samples
  for (int i = BUFWINDOW; i < MAXBUF; i++){
    redBuf[i - BUFWINDOW] = redBuf[i];
    irBuf[i - BUFWINDOW] = irBuf[i];
  }

  //take BUFWINDOW sets of samples before calculating the heart rate.
    for (byte i = MAXBUF-BUFWINDOW; i < MAXBUF; i++){
      
      // Wait until new data is available
      while (pulseOximeter.available() == false){
        pulseOximeter.check(); // Ask the sensor to check
      }
      
      redBuf[i] = pulseOximeter.getRed();
      irBuf[i] = pulseOximeter.getIR();
      pulseOximeter.nextSample(); // Prepare next sample

      Serial.print(F("red="));
      Serial.print(redBuf[i], DEC);
      Serial.print(F(", ir="));
      Serial.print(irBuf[i], DEC);
      Serial.print(F(", HR="));
      Serial.print(heartRate, DEC);
      Serial.print(F(", HRvalid="));
      Serial.print(validHeartRate, DEC);
      Serial.print(F(", SPO2="));
      Serial.print(spo2, DEC);
      Serial.print(F(", SPO2Valid="));
      Serial.println(validSpo2, DEC);
    }
    
    // Run the algorithm for SpO2 calculation
    maxim_heart_rate_and_oxygen_saturation(irBuf, bufLen, redBuf, &spo2, &validSpo2, &heartRate, &validHeartRate);
}
