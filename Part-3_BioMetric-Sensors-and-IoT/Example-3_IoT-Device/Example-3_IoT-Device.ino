#define LARGE_JSON_BUFFERS 1
#include <Thing.h>
#include <WebThingAdapter.h>
// Also requires ESPAsyncWebServer:
// git clone https://github.com/me-no-dev/ESPAsyncWebServer.git
// git clone https://github.com/me-no-dev/AsyncTCP.git

#define LEDPIN 4

const char *ssid = "workshop";
const char *password = "workshop";

ThingActionObject *action_generator(DynamicJsonDocument *);
WebThingAdapter *adapter;

const char *ledTypes[] = {"OnOffSwitch", "Light", nullptr};
ThingDevice led("led", "My LED Thing", ledTypes);
// Properties:
ThingProperty ledOn("on", "Whether the LED is turned on", BOOLEAN, "OnOffProperty");
ThingProperty ledLevel("brightness", "The level of light from 0-100", INTEGER, "BrightnessProperty");

bool prevOn = false;
int prevlevel = 100;
char printbuf[256] = "";

void setup() {
  // Initialize the led as turned off:
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);

  Serial.begin(115200);
  sprintf(printbuf, "Connecting to \"%s\"", ssid);
  Serial.println(printbuf);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  IPAddress myIPAddress = WiFi.localIP();
  char cStrIP[16];
  sprintf(cStrIP, "%d.%d.%d.%d", myIPAddress[0], myIPAddress[1], myIPAddress[2], myIPAddress[3]);
  
  sprintf(printbuf, "Connected to \"%s\"\nIP address: %s", ssid, cStrIP);
  Serial.println(printbuf);

  // Setup new adapter
  adapter = new WebThingAdapter("my-led", myIPAddress);

  // Set up LED:
  led.description = "A web connected LED";

  // On/Off controls:
  ledOn.title = "On/Off";
  led.addProperty(&ledOn);
  ThingPropertyValue initialOn = {.boolean = false};
  ledOn.setValue(initialOn);
  (void)ledOn.changedValueOrNull();

  //Brightness controls:
  ledLevel.title = "Brightness";
  ledLevel.minimum = 0;
  ledLevel.maximum = 100;
  ledLevel.unit = "percent";
  led.addProperty(&ledLevel);
  ThingPropertyValue initialLevel = {.integer = 50};
  ledLevel.setValue(initialLevel);
  (void)ledLevel.changedValueOrNull();

  // Add led to adapter and begin:
  adapter->addDevice(&led);
  adapter->begin();

  sprintf(printbuf, "HTTP server started on\"%s/things/%s\"",cStrIP,led.id);
  Serial.println(printbuf);
  
}

void loop() {
  adapter->update();
  bool onOff = ledOn.getValue().boolean;
  int level = ledLevel.getValue().integer;

  if (onOff != prevOn or level != prevlevel) {
    sprintf(printbuf, "%s : %d : %d", led.id, onOff, level);
    Serial.println(printbuf);
    if (onOff == true){
      analogWrite(LEDPIN, map(level, 0, 100, 0, 255));
    }
    else{
      analogWrite(LEDPIN, 0);
    }
  }
  prevOn = onOff;
  prevlevel = level;

}
