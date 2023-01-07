/**
 * Homie Node for LD2410 mmWave Radar Motion Sensor
 * 
 * GPIO pin for RCWL should have a pull down resistor to keep inactive state low
 * GPIO 16 has the INPUT_PULLDOWN_16 capability, but it is also the wakeup pin
 * 
 */

#pragma once

#include <Homie.hpp>
// #include "SoftwareSerial.h"
#include <ld2410.h>

#define SNAME "LD2410-Sensor"

class LD2410Client : public HomieNode {

public:
  LD2410Client(const char *id, const char *name, const char *nType, const uint8_t rxPin, const uint8_t txPin, const uint8_t ioPin);

  void setMotionHoldInterval(unsigned long interval) { _motionHoldInterval = interval; }
  unsigned long getMotionHoldInterval() const { return _motionHoldInterval; }

protected:
  virtual void setup() override;
  virtual void loop() override;
  virtual void onReadyToOperate() override;

  String availableCommands();
  void   commandHandler();
  String commandProcessor(String &cmdStr);
  String buildWithAlarmSerialStudioCSV();

private:
  // suggested rate is 1/60Hz (1m)
  static const int MIN_INTERVAL  = 10;  // in seconds
  static const int HOLD_INTERVAL = 60;

  const char *cCaption = "• LD2410 mmWave Radar Motion Sensor:";
  const char* cIndent  = "  ◦ ";

  // Motion Node Properties
  const uint8_t _rxPin;
  const uint8_t _txPin;
  const uint8_t _ioPin;
  
  unsigned long _lastReading = 0;

  const char *cProperty = "motion";
  const char *cPropertyName = "Motion";
  const char *cPropertyDataType = "enum";
  const char *cPropertyFormat = "ON,OFF";
  const char *cPropertyUnit = "";

  unsigned long _motionHoldInterval;
  
  // LD2410 Interface
  volatile bool _motion = false;
  volatile byte pin_gpio = LOW;
    
  // SoftwareSerial gpsSerial;
  ld2410 radar;

  volatile bool udpFlag = false; // send for callback
  uint32_t lastReading = 0;
  uint32_t pos = 0;
  uint32_t pos1 = 0;
  bool sending_enabled = false;
  String command = "";
  String output = "";
  char buffer1[128];
  char serialBuffer[256];

};
