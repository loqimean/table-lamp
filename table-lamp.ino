#include "HomeSpan.h"
#include "table-lamp-service.h"

const char* ssid     = "YourWiFiName";
const char* password = "YourWiFiPassword";

#define LAMP_PIN 5
#define STATUS_LED 2
#define BUTTON_PIN 4

void setup() {
  Serial.begin(9600);

  pinMode(BUTTON_PIN, OUTPUT);

  homeSpan.setStatusPin(STATUS_LED);
  homeSpan.setStatusAutoOff(10);

  homeSpan.setWifiCredentials(ssid, password);
  homeSpan.setPairingCode("2255533");

  homeSpan.begin(Category::Lighting,"HomeSpan Light");

  new SpanAccessory();           // Table Lamp Accessory

  new Service::AccessoryInformation();            // HAP requires every Accessory to implement an AccessoryInformation Service
  new Characteristic::Identify();               // HAP requires the Accessory Information Service to include the Identify Characteristic
  new TableLamp(LAMP_PIN, BUTTON_PIN);                              // instantiate the TableLamp Service (defined below) with lampPin set to 17
}

void loop() {
 homeSpan.poll();
}
