/**
 * Homie Node for DHT Temperature and Humidity  Sensor
 * 
 */
#include "DHTNode.hpp"

DHTNode::DHTNode(const uint8_t dhtPin, DHTesp::DHT_MODEL_t dhtModel, const char *id, const char *name, const char *nType, const int measurementInterval = MEASUREMENT_INTERVAL)
    : HomieNode(id, name, nType)
{
  _measurementInterval = (measurementInterval > MIN_INTERVAL) ? measurementInterval : MIN_INTERVAL;
  _lastMeasurement     = 0;

  // Start up the library
  sensor = new DHTesp();
  sensor->setup(dhtPin, dhtModel);
}

  /**
    * Called by Homie when Homie.setup() is called; Once!
  */
  void DHTNode::setup() {
    Homie.getLogger() << cIndent << F("Sensor Model:  ") << sensor->getModel() << endl;
    Homie.getLogger() << cIndent << F("Sensor Status: ") << sensor->getStatusString() << endl;
    

    advertise(cHumidity)
        .setName(cHumidityName)
        .setDatatype("float")
        .setUnit(cHumidityUnit);

    advertise(cTemperature)
      .setName(cTemperatureName)
      .setDatatype("float")
      .setUnit(cTemperatureUnit);
  }

  /**
   * Called by Homie when homie is connected and in run mode
  */
  void DHTNode::loop() {
    if (!(millis() - _lastMeasurement >= _measurementInterval * 1000UL || _lastMeasurement == 0)) {
      return;
    }
    _lastMeasurement = millis();

    Homie.getLogger() << F("〽 Sending Temperature: ") << getId() << endl;
    
    _sensorResults = sensor->getTempAndHumidity();
    _sensorStatus  = sensor->getStatus();
    if (_sensorStatus == DHTesp::ERROR_NONE)
    {
      Homie.getLogger() << cIndent
                        << F("Temperature=")
                        << getTemperatureF()
                        << F(", Humidity=")
                        << getHumidity()
                        << endl;
      setProperty(cTemperature).send(String( getTemperatureF() ));
      setProperty(cHumidity).send(String( getHumidity() ));
    }
    else
    {
      Homie.getLogger() << cIndent
                        << F("✖ Error reading sensor: ")
                        << sensor->getStatusString()
                        << ", value (F) read=" << _sensorResults.temperature
                        << endl;
    }
  }

