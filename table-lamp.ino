#include "HomeSpan.h"

const char* ssid     = "YourWiFiName";
const char* password = "YourWiFiPassword";

#define LAMP_PIN 5

struct TableLamp : Service::LightBulb {
  int lampPin;                               // store the pin number connected to a hypothetical relay that turns the Table Lamp on/off
  SpanCharacteristic *lampPower;             // store a reference to the On Characteristic

  TableLamp(int lampPin) : Service::LightBulb() {       // constructor() method for TableLamp defined with one parameter.  Note we also call the constructor() method for the LightBulb Service.
    lampPower=new Characteristic::On();      // instantiate the On Characteristic and save it as lampPower
    this->lampPin=lampPin;                   // save the pin number for the hypothetical relay

    pinMode(lampPin,OUTPUT);                 // configure the pin as an output using the standard Arduino pinMode function
  }

  boolean update() {
    digitalWrite(lampPin,lampPower->getNewVal());      // use standard Arduino digitalWrite function to change the ledPin to either high or low based on the value requested by HomeKit

    return(true);                            // return true to let HomeKit (and the Home App Client) know the update was successful
  }
};

void setup() {
  Serial.begin(9600);

  homeSpan.setWifiCredentials(ssid, password);
  homeSpan.setPairingCode("11111111");

  homeSpan.begin();

  new SpanAccessory();           // Table Lamp Accessory

  new Service::AccessoryInformation();            // HAP requires every Accessory to implement an AccessoryInformation Service
  new Characteristic::Identify();               // HAP requires the Accessory Information Service to include the Identify Characteristic
  new TableLamp(LAMP_PIN);                              // instantiate the TableLamp Service (defined below) with lampPin set to 17

}

void loop() {
 homeSpan.poll();
}
