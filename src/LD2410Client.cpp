/**
 * Homie Node for LD2410 mmWave Radar Motion Sensor
 * 
 */
#include "LD2410Client.hpp"

LD2410Client::LD2410Client(const char *id, const char *name, const char *nType, const uint8_t rxPin, const uint8_t txPin, const uint8_t ioPin, const bool enableReporting)
    : HomieNode(id, name, nType, false, 0U, 0U),
    _rxPin(rxPin),
    _txPin(txPin),
    _ioPin(ioPin),
    _reporting_enabled(enableReporting)
{
  // Start up the library
  pinMode(_ioPin, INPUT);
}

/**
 * Handles the received MQTT messages from Homie.
 *
 */
bool LD2410Client::handleInput(const HomieRange &range, const String &property, const String &value)
{
    Homie.getLogger() << "nodeInputHandler()  " << property << ": " << value << endl;

    if (property.equalsIgnoreCase(cPropertyCommand))
    {
      String _value = value;
      String _response =commandProcessor( _value );
       Homie.getLogger() << cIndent
                        << F("✖Command Response: ") << value << "\n"
                        << _response
                        << endl;
      setProperty(cPropertyCommand).setRetained(false).send(_response.c_str());
    }
   
    return true;
}

/**
 *
 */
void LD2410Client::onReadyToOperate()
{
  _motion = false;
  Homie.getLogger() << cCaption << endl;
  Homie.getLogger() << cIndent << "onReadyToOperate()" << endl;
}

/**
 * Called by Homie when Homie.setup() is called; Once!
*/
void LD2410Client::setup() {
  Homie.getLogger() << cCaption << endl;

  // start path to LD2410
  // radar.debug(Serial);  // enable debug output to console
  Serial2.begin(256000, SERIAL_8N1, _rxPin, _txPin); // UART for monitoring the radar rx, tx  

  if (radar.begin(Serial2)) {
    Homie.getLogger() << cCaption << "Initialized..." << endl;
  } else {
    Homie.getLogger() << cCaption << " was not connected" << endl;
  }

  _motion = (digitalRead(_ioPin) ? true : false);

  advertise(cPropertyMotion)
      .setName(cPropertyMotionName)
      .setDatatype(cPropertyMotionDataType)
      .setFormat(cPropertyMotionFormat)
      .setUnit(cPropertyMotionUnit);

  advertise(cPropertyCommand)
      .setName(cPropertyCommandName)
      .setDatatype(cPropertyCommandDataType)
      .setFormat(cPropertyCommandFormat)
      .settable();
}

/**
 * Called by Homie when homie is connected and in run mode
*/
void LD2410Client::loop() {
  uint32_t timer = millis();
  radar.ld2410_loop();
  
  // process reporting data
  if (radar.isConnected() && (timer - _lastReport) > _targetReportingInterval) // Report every 1000ms
  {
    _lastReport = timer;
    if (_reporting_enabled) {
      String pData = processTargetData();
      Homie.getLogger() << processTargetData() << endl;      
      setProperty(cPropertyCommand).setRetained(false).send(pData.c_str());
    }
  }
  
  // Module controls hold time, so all we need is to read the io pin
  if ((_motion != digitalRead(_ioPin)) || ((timer - _lastBroadcast) > _broadcastInterval)) {
        
    _lastBroadcast = timer;
    _motion = digitalRead(_ioPin);
    if (_motion) {
      Homie.getLogger() << cIndent
                        << F("✖ Motion Detected: ON ")
                        << endl;
      setProperty(cPropertyMotion).setRetained(true).send("ON");
    } else {
      Homie.getLogger() << cIndent
                        << F("✖ Motion Detected: OFF ")
                        << endl;
      setProperty(cPropertyMotion).setRetained(true).send("OFF");
    }
  }

  // Read from Serial, look for commands
  commandHandler();
}

// clang-format off
/*
 * Available Commands */
String LD2410Client::availableCommands() {
  String sCmd = "";
  sCmd += "\nNode: ";
  sCmd += cCaption;
  sCmd += "\nSupported commands:";
  sCmd += "\n\t( 1) help:         this text.";
  sCmd += "\n\t( 2) streamstart:  start sending udp data to SerialStudio.";
  sCmd += "\n\t( 3) streamstop:   stop sending to SerialStream.";
  sCmd += "\n\t( 4) read:         read current values from the sensor";
  sCmd += "\n\t( 5) readconfig:   read the configuration from the sensor";
  sCmd += "\n\t( 6) setmaxvalues   <motion gate> <stationary gate> <inactivitytimer> (2-8) (0-65535)seconds";
  sCmd += "\n\t( 7) setsensitivity <gate> <motionsensitivity> <stationarysensitivity> (2-8|255) (0-100)";
  sCmd += "\n\t( 8) restart:      restart the sensor";
  sCmd += "\n\t( 9) readversion:  read firmware version";
  sCmd += "\n\t(10) factoryreset: factory reset the sensor";
  sCmd += "\n\t(11) deviceinfo:   LD2410 device info";
  sCmd += "\n\t(12) reboot:       reboot hosting micro-controller\n";

  return sCmd;
}
// clang-format on

/*
 * Command Processor
 * - there are two commands not implemented here
 * - requestConfigurationModeBegin()
 * - requestConfigurationModeEnd()
 * Otherwise all commands are available as options
 */
String LD2410Client::commandProcessor(String &cmdStr) {
  String sBuf = "";
  int iCmd = cmdStr.toInt();
  cmdStr.trim();

  if (cmdStr.equals("help") || iCmd == 1) {
    sBuf += LD_CMD_OKNL;
    sBuf += availableCommands();
  } else if (cmdStr.equals("streamstart") || iCmd == 2) {
    _reporting_enabled = true;
    sBuf += LD_CMD_OK;
  } else if (cmdStr.equals("streamstop") || iCmd == 3) {
    _reporting_enabled = false;
    sBuf += LD_CMD_OK;
  } else if (cmdStr.equals("read") || iCmd == 4) {
    if (radar.isConnected()) {
      sBuf += LD_CMD_OKNL;
      if (radar.presenceDetected()) {
        if (radar.stationaryTargetDetected()) {
          sBuf += "Stationary target: ";
          sBuf += radar.stationaryTargetDistance();
          sBuf += " cm energy: ";
          sBuf += radar.stationaryTargetEnergy();
          sBuf += " dBZ\n";
        }
        if (radar.movingTargetDetected()) {
          sBuf += "Moving target: ";
          sBuf += radar.movingTargetDistance();
          sBuf += " cm energy: ";
          sBuf += radar.movingTargetEnergy();
          sBuf += " dBZ\n";
        }
        if (!radar.stationaryTargetDetected() &&
            !radar.movingTargetDetected()) {
          sBuf += "No Detection, in Idle Hold window of: ";
          sBuf += radar.cfgSensorIdleTimeInSeconds();
          sBuf += " seconds\n";
        }
      } else {
        sBuf += LD_CMD_OK;
      }
    } else {
      sBuf += LD_CMD_FAIL;
    }
  } else if (cmdStr.equals("readconfig") || iCmd == 5) {
    if (radar.requestCurrentConfiguration()) {
      sBuf += LD_CMD_OKNL;
      sBuf += "Maximum gate ID: ";
      sBuf += radar.cfgMaxGate();
      sBuf += "\n";
      sBuf += "Maximum gate for moving targets: ";
      sBuf += radar.cfgMaxMovingGate();
      sBuf += "\n";
      sBuf += "Maximum gate for stationary targets: ";
      sBuf += radar.cfgMaxStationaryGate();
      sBuf += "\n";
      sBuf += "Idle time for targets: ";
      sBuf += radar.cfgSensorIdleTimeInSeconds();
      sBuf += "s\n";
      sBuf += "Gate sensitivity\n";

      for (uint8_t gate = 0; gate < LD2410_MAX_GATES; gate++) {
        sBuf += "Gate ";
        sBuf += gate;
        sBuf += " moving targets: ";
        sBuf += radar.cfgMovingGateSensitivity(gate);
        sBuf += " dBZ stationary targets: ";
        sBuf += radar.cfgStationaryGateSensitivity(gate);
        sBuf += " dBZ\n";
      }
    } else {
      sBuf += LD_CMD_FAIL;
    }
  } else if (cmdStr.startsWith("setmaxvalues") || iCmd == 6) {
    uint8_t firstSpace = cmdStr.indexOf(' ');
    uint8_t secondSpace = cmdStr.indexOf(' ', firstSpace + 1);
    uint8_t thirdSpace = cmdStr.indexOf(' ', secondSpace + 1);

    uint8_t newMovingMaxDistance =
        (cmdStr.substring(firstSpace, secondSpace)).toInt();
    uint8_t newStationaryMaxDistance =
        (cmdStr.substring(secondSpace, thirdSpace)).toInt();
    uint16_t inactivityTimer =
        (cmdStr.substring(thirdSpace, cmdStr.length())).toInt();

    if (newMovingMaxDistance > 0 && newStationaryMaxDistance > 0 &&
        newMovingMaxDistance <= 8 && newStationaryMaxDistance <= 8) {
      if (radar.setMaxValues(newMovingMaxDistance, newStationaryMaxDistance,inactivityTimer)) {
        if (radar.requestRestart()) {
          delay(1500);
          if (radar.requestStartEngineeringMode()) {
            sBuf += LD_CMD_OK;
          }
        } else {
          sBuf += LD_CMD_FAIL;
        }
      } else {
        sBuf += LD_CMD_FAIL;
      }
    } else {
        sBuf += LD_CMD_FAIL;
    }
  } else if (cmdStr.startsWith("setsensitivity") || iCmd == 7) {
    uint8_t firstSpace = cmdStr.indexOf(' ');
    uint8_t secondSpace = cmdStr.indexOf(' ', firstSpace + 1);
    uint8_t thirdSpace = cmdStr.indexOf(' ', secondSpace + 1);

    uint8_t gate = (cmdStr.substring(firstSpace, secondSpace)).toInt();
    uint8_t motionSensitivity =
        (cmdStr.substring(secondSpace, thirdSpace)).toInt();
    uint8_t stationarySensitivity =
        (cmdStr.substring(thirdSpace, cmdStr.length())).toInt();

    // Command method 1 -- limit gate to 0-8 -- set one gate set
    // Command method 2 -- limit gate to 255 -- set all gates to same
    // sensitivity value
    if (motionSensitivity >= 0 && stationarySensitivity >= 0 &&
        motionSensitivity <= 100 && stationarySensitivity <= 100) {
      if (radar.setGateSensitivityThreshold(gate, motionSensitivity,stationarySensitivity)) {
        if (radar.requestRestart()) {
          delay(1500);
          if (radar.requestStartEngineeringMode()) {
            sBuf += LD_CMD_OK;
          }
        } else {
          sBuf += LD_CMD_FAIL;
        }
      } else {
        sBuf += LD_CMD_FAIL;
      }
    } else {
      sBuf += LD_CMD_FAIL;
    }
  } else if (cmdStr.equals("restart") || iCmd == 8) {
    if (radar.requestRestart()) {
      delay(1500);
      if (radar.requestStartEngineeringMode()) {
        sBuf += LD_CMD_OK;
      }
    } else {
      sBuf += LD_CMD_FAIL;
    }
  } else if (cmdStr.equals("readversion") || iCmd == 9) {
    if (radar.requestFirmwareVersion()) {
      sBuf += LD_CMD_OKNL;
      sBuf += radar.cmdFirmwareVersion();
    } else {
      sBuf += LD_CMD_FAIL;
    }
  } else if (cmdStr.equals("factoryreset") || iCmd == 10) {
    if (radar.requestFactoryReset()) {
      sBuf += LD_CMD_OK;      
    } else {
      sBuf += LD_CMD_FAIL;
    }
  } else if (cmdStr.equals("deviceinfo") || iCmd == 11) {
    sBuf += LD_CMD_OKNL;
    radar.requestFirmwareVersion();
    sBuf += "\n\tData reporting mode: ";
    sBuf += (radar.isEngineeringMode() ? "Engineering Mode" : "Target Mode");
    sBuf += "\n\tCommunication protocol version: v";
    sBuf += radar.cmdProtocolVersion();
    sBuf += ".0\n\tCommunications Buffer Size: ";
    sBuf += radar.cmdCommunicationBufferSize();
    sBuf += " bytes\n\tDevice firmare version: ";
    sBuf += radar.cmdFirmwareVersion();
    sBuf += "\tEngineering retain data value: ";
    sBuf += radar.engRetainDataValue();
    sBuf += "\n";
  } else if (cmdStr.equals("reboot") || iCmd == 12) {
    esp_restart();
  } else {
    sBuf += LD_CMD_FAILNL;
    sBuf += "Unknown command: ";
    sBuf += cmdStr;
    sBuf += "\n";
  }

  cmdStr.clear();

  return sBuf;
}

/*
 * Accepts Serial chars and process chars as a command
 * when the newline char is received
 */
void LD2410Client::commandHandler() {
  if (Serial.available()) {
    char typedCharacter = Serial.read();
    if (typedCharacter == '\n') {
      Serial.print(commandProcessor(command));
    } else {
      Serial.print(typedCharacter);
      if (typedCharacter != '\r') { // effectively ignore CRs
        command += typedCharacter;
      }
    }
  }
}

/*
 * Translate current status
*/
const char * LD2410Client::triggeredby() {

  if(radar.isMoving() && radar.isStationary()) {
    triggered = "Stationary and Moving";
  } else if (radar.isStationary()) {
    triggered = "Stationary";
  } else if(radar.isMoving()) {
    triggered = "Moving";
  } else {
    triggered = "Not Present";
  }

  return triggered.c_str();
}

String LD2410Client::processTargetData() {
  if(radar.isEngineeringMode()) {
    return processEngineeringReportData();
  } else {
    return processTargetReportingData();
  }
}

// clang-format off
String LD2410Client::processTargetReportingData() {
  uint32_t pos = 0;
  pos = snprintf(serialBuffer, sizeof(serialBuffer),
                 "{\"name\":\"%s\",\"triggeredBy\":\"%s\",\"detectionDistanceFT\":%3.1f,\"movingTargetDistanceFT\":%3.1f,\"movingTargetEnergy\":%d,\"stationaryTargetDistanceFT\":%3.1f,\"stationaryTargetEnergy\":%d}", 
                 cCaption, triggeredby(),
                 (radar.detectionDistance() * CM_TO_FEET_FACTOR),
                 (radar.movingTargetDistance() * CM_TO_FEET_FACTOR) , radar.movingTargetEnergy(),
                 (radar.stationaryTargetDistance() * CM_TO_FEET_FACTOR), radar.stationaryTargetEnergy() );

  return String(serialBuffer);
}

// clang-format off
/*
 * CSV like Values for SerialStudio App - see test folder */
//              %1,2,3, 4, 5,6,7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44
/*LD2410 Sensor 01,0,0,62,43,0,0,00,50,15, 0, 0,50,15, 0, 0,40, 5,40,62,30, 9,40,45,20, 3,30,25,15, 6,30,18,15, 1,20,10,15, 2,20, 8,15, 7,20, 6*/
// clang-format on
String LD2410Client::processEngineeringReportData() {
  uint32_t pos = 0;
  uint32_t pos1 = 0;
  pos = snprintf(serialBuffer, sizeof(serialBuffer),
                 "/*%s,%d,%d,%d,%d,%d,%d,%d,%d,", cCaption,
                 radar.stationaryTargetDistance(), radar.detectionDistance(),
                 radar.stationaryTargetEnergy(), radar.movingTargetDistance(),
                 radar.detectionDistance(), radar.movingTargetEnergy(),
                 radar.engRetainDataValue(), (_motion ? 100 : 0));

  for (int x = 0; x < LD2410_MAX_GATES; ++x) {
    pos1 = snprintf(buffer1, sizeof(buffer1), "%d,%d,%d,%d,",
                    radar.cfgMovingGateSensitivity(x),
                    radar.engMovingDistanceGateEnergy(x),
                    radar.cfgStationaryGateSensitivity(x),
                    radar.engStaticDistanceGateEnergy(x));
    strcat(serialBuffer, buffer1);
    pos += pos1;
  }
  strcat(serialBuffer, "*/\n");

  return String(serialBuffer);
}
