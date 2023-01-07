/**
 * @file main.cpp
 * @author James Scott, Jr.  aka(Skoona) (skoona@gmail.com)
 * @brief HomieNode featuring an DHT (11/22) Temperature sensor.
 * @version 1.0.0
 * @date 2021-01-31
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <Arduino.h>
#include "DHTNode.hpp"
#include "LD2410Client.hpp"
#include "MetricsNode.hpp"

#ifdef ESP8266
extern "C"
{
#include "user_interface.h" // to set CPU Freq for Non-Huzzah's
}
#endif

/*
 * SKN_NODE_ID becomes the base name when ranges are used
 * ex: sknSensors/deviceName/DHT_0/temperature -> 72.3 degress
 * Note: HomieNode(...range,lower,upper) manages this array suffix change; i.e no more name fixups
*/
#define SKN_MOD_NAME "Monitor-DHT-mmWave-Metrics"
#define SKN_MOD_VERSION "2.0.2"
#define SKN_MOD_BRAND "SknSensors"

#define SKN_TNODE_TITLE "Temperature and Humidity Sensor"
#define SKN_MNODE_TITLE "Occupancy and Motion Sensor"

#define SKN_TNODE_TYPE "Sensor"
#define SKN_MNODE_TYPE "Sensor"

#define SKN_TNODE_ID "Ambient"    
#define SKN_MNODE_ID "Occupancy"

#define SKN_DNODE_TITLE "Device Info"
#define SKN_DNODE_TYPE "Sensor"
#define SKN_DNODE_ID "Hardware"

// Select pins for Temperature and Motion
#define PIN_DHT  D6                  // 12
#define LD_IO D5                  // 14
#define LD_RX D7 // 13
#define LD_TX D8 // 15
#define DHT_TYPE DHTesp::DHT_MODEL_t::DHT22 // DHTesp::DHT22

#define SENSOR_READ_INTERVAL 300

ADC_MODE(ADC_VCC); //vcc read in MetricsNode

HomieSetting<long> sensorsIntervalSetting("sensorInterval", "The interval in seconds to wait between sending commands.");
HomieSetting<long> broadcastInterval("broadcastInterval", "how frequently to send pin values.");
HomieSetting<long> dhtType("dhtType", "Type os Humidty Sensor where; DHTesp::DHT_MODEL_t::DHT11 = 1. DHTesp::DHT_MODEL_t::DHT22 = 2");

DHTNode temperatureMonitor(PIN_DHT, DHT_TYPE, SKN_TNODE_ID, SKN_TNODE_TITLE, SKN_TNODE_TYPE, SENSOR_READ_INTERVAL);
LD2410Client  motionMonitor(SKN_MNODE_ID, SKN_MNODE_TITLE, SKN_MNODE_TYPE, LD_RX, LD_TX, LD_IO);
MetricsNode metricsNode(SKN_DNODE_ID, SKN_DNODE_TITLE, SKN_DNODE_TYPE);

void setup()
{  
  delay(250);  
  Serial.begin(115200);
  delay(250);  
  Serial << endl << "Starting..." << endl;

  sensorsIntervalSetting.setDefaultValue(180).setValidator([](long candidate) {
    return (candidate >= 10) && (candidate <= 3000);
  });
  broadcastInterval.setDefaultValue(60).setValidator([](long candidate) {
    return (candidate >= 10) && (candidate <= 361);
  });
  dhtType.setDefaultValue(2).setValidator([](long candidate) {
    return (candidate >= 0) && (candidate <= 4);
  });

  temperatureMonitor.setMeasurementInterval(sensorsIntervalSetting.get());
  temperatureMonitor.setModel((DHTesp::DHT_MODEL_t)dhtType.get());
  metricsNode.setMeasurementInterval(60);

  Homie_setFirmware(SKN_MOD_NAME, SKN_MOD_VERSION);
  Homie_setBrand(SKN_MOD_BRAND);

  Homie.setup();
}

void loop()
{
  Homie.loop();
}
