#define SHORT_TOUCH 5         // Short touch duration in milliseconds

Preferences preferences;

struct TableLamp : Service::LightBulb {
  int lampPin;                               // store the pin number connected to a hypothetical relay that turns the Table Lamp on/off
  int powerButtonPin;

  SpanCharacteristic *lampPower;             // store a reference to the On Characteristic
  Preferences preferences;

  unsigned long lastTouchTime = 0;           // Time of the last touch event
  unsigned long touchStartTime = 0;          // Time when a touch started
  bool isTouched = false;                    // Current touch state

  TableLamp(int lampPin, int powerButtonPin) : Service::LightBulb() {       // constructor() method for TableLamp defined with one parameter.  Note we also call the constructor() method for the LightBulb Service.
    lampPower = new Characteristic::On();      // instantiate the On Characteristic and save it as lampPower
    
    this->lampPin = lampPin;                   // save the pin number for the hypothetical relay
    this->powerButtonPin = powerButtonPin;
    this->preferences = preferences;

    // new SpanButton(powerButtonPin, SpanButton::TRIGGER_ON_TOUCH);

    pinMode(lampPin, OUTPUT);                 // configure the pin as an output using the standard Arduino pinMode function
    pinMode(powerButtonPin, INPUT);

    preferences.begin("TableLamp", false);    // Open preferences with the namespace "TableLamp"

    // Restore the lamp state from preferences
    bool savedLampState = preferences.getBool("lampState", false);
    lampPower->setVal(savedLampState);
    digitalWrite(lampPin, savedLampState);

    LOG1("Restored lamp state: ");
    LOG1(savedLampState ? "true" : "false");
  }

  boolean update() {
    if (lampPower->updated()) {
      LOG1("  New Power=");
      LOG1(lampPower->getNewVal() ? "true" : "false");

      // Save the new state to preferences
      preferences.putBool("lampState", lampPower->getNewVal());
    }

    digitalWrite(lampPin, lampPower->getNewVal());      // use standard Arduino digitalWrite function to change the ledPin to either high or low based on the value requested by HomeKit

    return(true);                            // return true to let HomeKit (and the Home App Client) know the update was successful
  } // update

  void loop() {
    int buttonState = digitalRead(powerButtonPin);
    unsigned long currentTime = millis();

    if (buttonState == HIGH && !isTouched) {
      isTouched = true;
      touchStartTime = currentTime;
    } else if (buttonState == LOW && isTouched) {
      isTouched = false;
      unsigned long touchDuration = currentTime - touchStartTime;

      if (touchDuration < SHORT_TOUCH) {
        // Debounce, ignore too short touches
        return;
      }

      LOG1("Touch detected.\n");

      int newValue = 1 - lampPower->getVal();

      // Update Apple Home app state
      lampPower->setVal(newValue);

      // Update memorized state
      preferences.putBool("lampState", newValue);

      // Update state of Lamp
      digitalWrite(lampPin, newValue);

      lastTouchTime = currentTime;
    }
  }
};
