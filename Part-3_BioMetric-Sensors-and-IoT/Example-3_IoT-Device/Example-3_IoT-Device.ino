#define LARGE_JSON_BUFFERS 1 // #define for webthing settings

// Webthing libraries:
#include <Thing.h>
#include <WebThingAdapter.h>
// Also requires ESPAsyncWebServer:
// Download from https://github.com/me-no-dev/ESPAsyncWebServer
// Download from https://github.com/me-no-dev/AsyncTCP

#define LEDPIN 4

// We set the SSID and password for our WiFi connection, where the webthings gateway is present:
const char *ssid = "workshop";
const char *password = "workshop";

// The WebThingAdapter is the interface WebThings can talk to us through
WebThingAdapter *adapter;

// We'll set up an LED that can be turned on and off, which can be represented as both OnOffSwitch and Light in WebThings:
const char *ledTypes[] = {"OnOffSwitch", "Light", nullptr};
ThingDevice led("led", "My LED Thing", ledTypes);

// Properties are defined, to be added in setup():
ThingProperty ledOn("on", "Whether the LED is turned on", BOOLEAN, "OnOffProperty");
ThingProperty ledLevel("brightness", "The level of light from 0-100", INTEGER, "BrightnessProperty");

// Internal variables used to program our behavior:
bool prevOn = false; // State for on/off
int prevlevel = 100; // Brightness level between 0-100
char printbuf[256] = ""; // 

void setup() {
  // Initialize the led as turned off:
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);

  // Specify On/Off controls, with the ledOn ThingProperty object:
  ledOn.title = "On/Off";
  ThingPropertyValue initialOn = {.boolean = false}; // Initial on/off value, to be shown on WebThings
  ledOn.setValue(initialOn); // Set the value
  (void)ledOn.changedValueOrNull();

  // Then specify Brightness controls, with the ledOn ThingProperty object:
  ledLevel.title = "Brightness";
  ledLevel.minimum = 0;
  ledLevel.maximum = 100;
  ledLevel.unit = "percent";
  ThingPropertyValue initialLevel = {.integer = 50}; // Initial level, to be shown on WebThings
  ledLevel.setValue(initialLevel); // Set the value
  (void)ledLevel.changedValueOrNull();

  // Set up LED, first with a description:
  led.description = "A web connected LED";
  // Add the two ThingProperty objects to the led device:
  led.addProperty(&ledOn);
  led.addProperty(&ledLevel);

  // Begin serial !!! It is 115200 baud !!!
  Serial.begin(115200);
  Serial.print("Connecting to "); Serial.println(ssid);

  // We connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print("."); // While waiting for connection, print dots
  }

  IPAddress myIPAddress = WiFi.localIP(); // We store the local IP adress
  
  // Print connection details:
  Serial.print("Connected to "); Serial.println(ssid);
  Serial.print("IP address: "); Serial.println(myIPAddress);

  // Setup adapter, with a name and IP to use:
  adapter = new WebThingAdapter("My-IoT-Device", myIPAddress);
  // Add led to adapter and begin:
  adapter->addDevice(&led);
  adapter->begin();

  // Write over UART where to find the Thing:
  Serial.print("HTTP server started on "); Serial.print(myIPAddress); Serial.print("/things/"); Serial.println(led.id);
}

void loop() {
  // Check for updates:
  adapter->update();

  // local variables onOff and level get the updated values from the server:
  bool onOff = ledOn.getValue().boolean;
  int level = ledLevel.getValue().integer;

  // If changed, update the LED output accordingly:
  if (onOff != prevOn || level != prevlevel) { // "or" can be both represented by "||" and "or"
    
    Serial.print("LED settings - "); Serial.print("onOff: "); Serial.print(onOff); Serial.print(" level: "); Serial.println(level);
    // Write to the LED:
    if (onOff == true){
      analogWrite(LEDPIN, map(level, 0, 100, 0, 255));
    }
    else{
      analogWrite(LEDPIN, 0);
    }
  }
  // Update prev values:
  prevOn = onOff;
  prevlevel = level;
}
