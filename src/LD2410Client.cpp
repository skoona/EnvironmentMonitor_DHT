/**
 * Homie Node for LD2410 mmWave Radar Motion Sensor
 * 
 */
#include "LD2410Client.hpp"

LD2410Client::LD2410Client(const char *id, const char *name, const char *nType, const uint8_t rxPin, const uint8_t txPin, const uint8_t ioPin)
    : HomieNode(id, name, nType, false, 0U, 0U),
    _rxPin(rxPin),
    _txPin(txPin),
    _ioPin(ioPin);
{
  // Start up the library
  pinMode(_ioPin, INPUT);
}

/**
 *
 */
void LD2410Client::onReadyToOperate()
{
  _isrLastTrigger = LOW;
  _motion = false;
  _isrTriggeredAt = 0L;

  Homie.getLogger() << cCaption << endl;
  Homie.getLogger() << cIndent << "onReadyToOperate()" << endl;
}

/**
 * Called by Homie when Homie.setup() is called; Once!
*/
void LD2410Client::setup() {
  Homie.getLogger() << cCaption << endl;
  Homie.getLogger() << cIndent << cPropertyName << endl;

// Create aa SerialPort
  gpsSerial = new  SoftwareSerial(_rxPin, _txPin, false, 128, true);

 // Start LD2410 Sensor
  if (radar.begin(gpsSerial)) {
    Homie.getLogger() << "Sensor Initialized..." << endl;
    delay(500);
    radar.requestStartEngineeringMode();
  } else {
    Homie.getLogger() <<" Sensor was not connected" << endl;
  }

  pin_gpio = (digitalRead(_ioPin) ? true : false);

  advertise(cProperty)
      .setName(cPropertyName)
      .setDatatype(cPropertyDataType)
      .setFormat(cPropertyFormat)
      .setUnit(cPropertyUnit);
}

/**
 * Called by Homie when homie is connected and in run mode
*/
void LD2410Client::loop() {
  radar.ld2410_loop();

   if (sending_enabled) {
    if (radar.isConnected() &&
        millis() - lastReading > 1000) // Report every 1000ms
    {
      lastReading = millis();
      if (Serial.available()) {
        Serial.print(buildWithAlarmSerialStudioCSV());
      }
      if (pin_gpio != (digitalRead(PIN_GPIO) ? true : false)) {
        pin_gpio = (digitalRead(PIN_GPIO) ? true : false);
      }
    }
  }
  commandHandler();

    if (!_motion)
    {
      _motion = true;

      Homie.getLogger() << F("〽 Sending Presence: ") << endl;

      Homie.getLogger() << cIndent
                        << F("✖ Motion Detected: ON ")
                        << endl;

      setProperty(cProperty).setRetained(true).send("ON");
    }
  }

  if (_isrTriggeredAt != 0 ) 
  {
    if (_motion && (millis() - _isrTriggeredAt >= _motionHoldInterval * 1000UL)) {
      // hold time expired

      _motion = false;          // re-enable motion
      _isrTriggeredAt = 0;

      Homie.getLogger() << cIndent
                        << F("✖ Motion Detected: OFF ")
                        << endl;

      setProperty(cProperty).setRetained(true).send("OFF");
    }
  }
}

