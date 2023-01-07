/**
 * Homie Node for LD2410 mmWave Radar Motion Sensor
 * 
 */
#include "LD2410Client.hpp"

LD2410Client::LD2410Client(const char *id, const char *name, const char *nType, const uint8_t rxPin, const uint8_t txPin, const uint8_t ioPin)
    : HomieNode(id, name, nType, false, 0U, 0U),
    _rxPin(rxPin),
    _txPin(txPin),
    _ioPin(ioPin),
    gpsSerial(_rxPin, _txPin, false, 128, true)
{
  // Start up the library
  pinMode(_ioPin, INPUT);
}

/**
 *
 */
void LD2410Client::onReadyToOperate()
{
  _motion = true;
  Homie.getLogger() << cCaption << endl;
  Homie.getLogger() << cIndent << "onReadyToOperate()" << endl;
}

/**
 * Called by Homie when Homie.setup() is called; Once!
*/
void LD2410Client::setup() {
  Homie.getLogger() << cCaption << endl;
  Homie.getLogger() << cIndent << cPropertyName << endl;

// Start the Software Serial Port
  gpsSerial.begin(256000);
  
  // gpsSerial.listen();

  while(!gpsSerial.available()) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();


 // Start LD2410 Sensor
  radar.debug(Serial); 

  if (radar.begin(gpsSerial)) {
    Homie.getLogger() << "Sensor Initialized..." << endl;
    delay(500);
    radar.requestStartEngineeringMode();
  } else {
    Homie.getLogger() <<" Sensor was not connected" << endl;
  }

  _motion = (digitalRead(_ioPin) ? true : false);

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
  gpsSerial.listen();

  radar.ld2410_loop();

  if (radar.isConnected() && millis() - _lastReading > 1000) // Report every 1000ms
  {
    _lastReading = millis();
    if (gpsSerial.available()) {
      Serial.print(buildWithAlarmSerialStudioCSV());
      
    }
    if (_motion != (digitalRead(_ioPin) ? true : false)) {
      _motion = (digitalRead(_ioPin) ? true : false);
      if (_motion) {
        Homie.getLogger() << F("〽 Sending Presence: ") << endl;
        Homie.getLogger() << cIndent
                          << F("✖ Motion Detected: ON ")
                          << endl;
        setProperty(cProperty).setRetained(true).send("ON");
      } else {
        Homie.getLogger() << F("〽 Sending Presence: ") << endl;
        Homie.getLogger() << cIndent
                          << F("✖ Motion Detected: OFF ")
                          << endl;
        setProperty(cProperty).setRetained(true).send("OFF");
      }
    }
  }
  commandHandler();
}

// clang-format off
/*
 * Available Commands */
String LD2410Client::availableCommands() {
  String sCmd = "";
  sCmd += "\nNode: ";
  sCmd += SNAME;
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
  String sBuf = "\n";
  int iCmd = cmdStr.toInt();
  cmdStr.trim();

  if (cmdStr.equals("help") || iCmd == 1) {
    sBuf += availableCommands();
  } else if (cmdStr.equals("streamstart") || iCmd == 2) {
    sending_enabled = true;
    sBuf += "\nSerialStudio UDP Stream Enabled. \n";
  } else if (cmdStr.equals("streamstop") || iCmd == 3) {
    sending_enabled = false;
    sBuf += "\nSerialStudio UDP Stream Disabled.\n";
  } else if (cmdStr.equals("read") || iCmd == 4) {
    sBuf += "\nReading from sensor: ";
    if (radar.isConnected()) {
      sBuf += "OK\n";
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
        sBuf += "\nnothing detected\n";
      }
    } else {
      sBuf += "failed to read\n";
    }
  } else if (cmdStr.equals("readconfig") || iCmd == 5) {
    sBuf += "\nReading configuration from sensor: ";
    if (radar.requestCurrentConfiguration()) {
      sBuf += "OK\n";
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
      sBuf += "Failed\n";
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
      sBuf += "\nSetting max values to gate ";
      sBuf += newMovingMaxDistance;
      sBuf += " moving targets, gate ";
      sBuf += newStationaryMaxDistance;
      sBuf += " stationary targets, ";
      sBuf += inactivityTimer;
      sBuf += "s inactivity timer: ";
      if (radar.setMaxValues(newMovingMaxDistance, newStationaryMaxDistance,
                             inactivityTimer)) {
        sBuf += "OK, now restart to apply settings\n";
      } else {
        sBuf += "failed\n";
      }
    } else {
      sBuf += "Can't set distances to ";
      sBuf += newMovingMaxDistance;
      sBuf += " moving ";
      sBuf += newStationaryMaxDistance;
      sBuf += " stationary, try again\n";
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
      sBuf += "\nSetting gate ";
      sBuf += gate;
      sBuf += " motion sensitivity to ";
      sBuf += motionSensitivity;
      sBuf += " dBZ & stationary sensitivity to ";
      sBuf += stationarySensitivity;
      sBuf += " dBZ: \n";
      if (radar.setGateSensitivityThreshold(gate, motionSensitivity,
                                            stationarySensitivity)) {
        sBuf += "OK, now restart to apply settings\n";
      } else {
        sBuf += "failed\n";
      }
    } else {
      sBuf += "Can't set gate ";
      sBuf += gate;
      sBuf += " motion sensitivity to ";
      sBuf += motionSensitivity;
      sBuf += " dBZ & stationary sensitivity to ";
      sBuf += stationarySensitivity;
      sBuf += " dBZ, try again\n";
    }
  } else if (cmdStr.equals("restart") || iCmd == 8) {
    if (radar.requestRestart()) {
      delay(1500);
      if (radar.requestStartEngineeringMode()) {
        sBuf += "\nRestarting sensor: OK\n";
      }
    } else {
      sBuf += "\nRestarting sensor: failed\n";
    }
  } else if (cmdStr.equals("readversion") || iCmd == 9) {
    sBuf += "\nRequesting firmware version: ";
    if (radar.requestFirmwareVersion()) {
      sBuf += radar.cmdFirmwareVersion();
    } else {
      sBuf += "Failed\n";
    }
  } else if (cmdStr.equals("factoryreset") || iCmd == 10) {
    sBuf += "\nFactory resetting sensor: ";
    if (radar.requestFactoryReset()) {
      sBuf += "OK, now restart sensor to take effect\n";
    } else {
      sBuf += "failed\n";
    }
  } else if (cmdStr.equals("deviceinfo") || iCmd == 11) {
    sBuf += "\nLD2410 Device Information for Node: ";
    sBuf += SNAME;
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
    ESP.restart();
  } else {
    sBuf += "\nUnknown command: ";
    sBuf += cmdStr;
    sBuf += "\n";
  }

  cmdStr.clear();
  sBuf += "\n choose:> ";

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


// clang-format off
/*
 * CSV like Values for SerialStudio App - see test folder */
//              %1,2,3, 4, 5,6,7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44
/*LD2410 Sensor 01,0,0,62,43,0,0,00,50,15, 0, 0,50,15, 0, 0,40, 5,40,62,30, 9,40,45,20, 3,30,25,15, 6,30,18,15, 1,20,10,15, 2,20, 8,15, 7,20, 6*/
// clang-format on
String LD2410Client::buildWithAlarmSerialStudioCSV() {
  uint32_t pos = 0;
  uint32_t pos1 = 0;
  pos = snprintf(serialBuffer, sizeof(serialBuffer),
                 "/*%s,%d,%d,%d,%d,%d,%d,%d,%d,", SNAME,
                 radar.stationaryTargetDistance(), radar.detectionDistance(),
                 radar.stationaryTargetEnergy(), radar.movingTargetDistance(),
                 radar.detectionDistance(), radar.movingTargetEnergy(),
                 radar.engRetainDataValue(), (pin_gpio ? 100 : 0));

  for (int x = 0; x < LD2410_MAX_GATES; ++x) {
    pos1 = snprintf(buffer1, sizeof(buffer1), "%d,%d,%d,%d,",
                    radar.cfgMovingGateSensitivity(x),
                    radar.engMovingDistanceGateEnergy(x),
                    radar.cfgStationaryGateSensitivity(x),
                    radar.engStaticDistanceGateEnergy(x));
    strcat(serialBuffer, buffer1);
    pos += pos1;
  }
  // serialBuffer[--pos] = 0;
  strcat(serialBuffer, "*/\n");

  return String(serialBuffer);
}
