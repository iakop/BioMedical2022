#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>

// Touch screen calibration
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

// Where the sensor is mounted
#define SENSOR_PIN A0

// Color cycle
#define CYCLE 256
#define MAXCOLOR 255

// Touch screen chip-select
#define TS_CS 7
XPT2046_Touchscreen ts(TS_CS); // Touch screen object

// TFT screen Chip select and data:
#define TFT_DC 8
#define TFT_CS 10
#define TFT_RST 9
#define TFT_BL 6
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST); // Display object

#define PENSIZE 3

void setup() {
  // Begin tft and touchscreen
  tft.begin();
  ts.begin();

  pinMode(TFT_BL, OUTPUT); // Set up the backlight
  digitalWrite(TFT_BL, HIGH); // Turn it on
  
  tft.setRotation(0);
  ts.setRotation(2); // For some reason their defaults are 180 deg offset.

  pinMode(SENSOR_PIN, INPUT); // Set sensor pin
  
  tft.fillScreen(ILI9341_BLACK); // Black background

  Serial.begin(115200);
}

void loop() {
  TS_Point tp = ts.getPoint(); // Touchpoint gets updated every loop

  // Touchpoint data is from 0-4000, and will be scaled down to fit screen:
  int touchX = map(tp.x, TS_MINX, TS_MAXX, 0, tft.width());
  int touchY = map(tp.y, TS_MINY, TS_MAXY, 0, tft.height());

  int red = 0, green = 0, blue = 0;
  
  setColors(&red, &green, &blue);
  
  // Draw the touched area:
  tft.fillCircle(touchX, touchY, PENSIZE, tft.color565(red, green, blue));
}

void setColors(int * red, int * green, int * blue){
  
  // Get a value to define color
  int sensorVal = analogRead(SENSOR_PIN);
  int colorVal = map(sensorVal, 0, 1023, 0, CYCLE*6);
  
  if (colorVal < CYCLE*1){
    *red = MAXCOLOR;
    *green = colorVal;
  }
  else if (colorVal < CYCLE*2){
    *red = CYCLE*1-(colorVal-CYCLE*1);
    *green = MAXCOLOR;
  }
  else if (colorVal < CYCLE*3){
    *green = MAXCOLOR;
    *blue = colorVal-CYCLE*2;
  }
  else if (colorVal < CYCLE*4){
    *green = CYCLE*1-(colorVal-CYCLE*3);
    *blue = MAXCOLOR;
  }
  else if (colorVal < CYCLE*5){
    *blue = MAXCOLOR;
    *red = colorVal-CYCLE*4;
  }
  else if (colorVal < CYCLE*6){
    *blue = CYCLE*1-(colorVal-CYCLE*5);
    *red = MAXCOLOR;
  }
  
  char printbuf[255];
  sprintf(printbuf, "Red: %d Green: %d Blue: %d", *red, *green, *blue);
  Serial.println(printbuf);
}
