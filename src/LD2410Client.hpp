/**
 * Homie Node for LD2410 mmWave Radar Motion Sensor
 * 
 * GPIO pin for RCWL should have a pull down resistor to keep inactive state low
 * GPIO 16 has the INPUT_PULLDOWN_16 capability, but it is also the wakeup pin
 * 
 */

#pragma once

#include <Homie.hpp>
#include <ld2410.h>

#define LD_CMD_OK "0"
#define LD_CMD_OKNL "0\n"
#define LD_CMD_FAIL "1"
#define LD_CMD_FAILNL "1\n"
#define CM_TO_FEET_FACTOR  0.0328084
#define LD_OCCUPANCY_TARGET_JSON "OccupancyTarget"
#define LD_OCCUPANCY_ENGINEERING_JSON "OccupancyEngineering"

class LD2410Client : public HomieNode {

public:
  LD2410Client(const char *id, const char *name, const char *nType, const uint8_t rxPin, const uint8_t txPin, const uint8_t ioPin, const bool enableReporting = false, const bool engineeringMode= false);

  void setTargetReportingInterval(unsigned long interval) { _targetReportingInterval = interval; }
  unsigned long getTargetReportingInterval() const { return _targetReportingInterval; }
  void setBroadcastInterval(unsigned long interval) { _broadcastInterval = interval; }
  unsigned long getBroadcastInterval() const { return _broadcastInterval; }
  void setTargetReporting(bool enabled) { _reporting_enabled = enabled; }
  bool isTargetReportingEnabled() const { return _reporting_enabled; }

protected:
  virtual void setup() override;
  virtual void loop() override;
  virtual void onReadyToOperate() override;
  virtual bool handleInput(const HomieRange &range, const String &property, const String &value);

  const char * triggeredby();
  String availableCommands();
  void   commandHandler();
  String commandProcessor(String &cmdStr);
  String processTargetData();
  String processTargetReportingData();
  String processEngineeringReportData();

private:
  const char *cCaption = "LD2410 mmWave Radar Motion Sensor:";
  const char* cIndent  = "  ◦ ";

  // Motion Node Properties
  const uint8_t _rxPin;
  const uint8_t _txPin;
  const uint8_t _ioPin;
  bool _reporting_enabled = false;
  bool _engineering_mode = false;

  const char *cPropertyMotion = "motion";
  const char *cPropertyMotionName = "Motion";
  const char *cPropertyMotionDataType = "enum";
  const char *cPropertyMotionFormat = "ON,OFF";
  const char *cPropertyMotionUnit = "";

  const char *cPropertyCommand = "system";
  const char *cPropertyCommandName = "Command Handler";
  const char *cPropertyCommandDataType = "string";
  const char *cPropertyCommandFormat = "";

  const char *cPropertyStatus = "occupancy";
  const char *cPropertyStatusName = "Occupancy";
  const char *cPropertyStatusDataType = "json";
  const char *cPropertyStatusFormat = "";

  // Loop Interval Parms
  uint32_t _targetReportingInterval = 15000;
  uint32_t _broadcastInterval = 15000;
  uint32_t _lastReport = 0;
  uint32_t _lastBroadcast = 0;

  // LD2410 Interface
  volatile bool _motion = false;
  ld2410 radar;

  // Command Processor Parms
  String command = "";
  String output = "";
  String triggered = "";
  char buffer1[128];
  char serialBuffer[3172];

};
