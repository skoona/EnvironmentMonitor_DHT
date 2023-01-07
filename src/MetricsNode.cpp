/**
 * Homie Node to overcome OpenHAB's lack of support for device metrics
 * 
 */
#include "MetricsNode.hpp"

MetricsNode::MetricsNode(const char *id, const char *name, const char *nType, const int measurementInterval)
    : HomieNode(id, name, nType, false, 0U, 0U)
{
  _measurementInterval = (measurementInterval > MIN_INTERVAL) ? measurementInterval : MIN_INTERVAL;
  _lastMeasurement     = 0;

}


/**
    * Called by Homie when Homie.setup() is called; Once!
  */
void MetricsNode::setup()
{
  Homie.getLogger() << cCaption
                    << endl;
  Homie.getLogger() << cIndent
                    << F("RSSI: ")
                    << WiFi.RSSI()
                    << F(" MAC: ")
                    << WiFi.macAddress().c_str()
                    << F(" Reset Reason: ")
                    << esp_rom_get_reset_reason(0)
                    << endl;

  advertise(cPropertySignal)
      .setName(cPropertySignalName)
      .setDatatype(cPropertySignalDataType)
      .setUnit(cPropertySignalFormat);

  advertise(cPropertyMac)
      .setName(cPropertyMac)
      .setDatatype(cPropertyMacDataType)
      .setUnit(cPropertyMacFormat);

  advertise(cPropertyResetReason)
      .setName(cPropertyResetReasonName)
      .setDatatype(cPropertyResetReasonDataType)
      .setUnit(cPropertyResetReasonFormat);
}

  /**
   * Called by Homie when homie is connected and in run mode
  */
void MetricsNode::loop() {
  if (millis() - _lastMeasurement >= (_measurementInterval * 1000UL) || _lastMeasurement == 0) {
    _lastMeasurement = millis();

    Homie.getLogger() << cIndent 
                      << F("〽 Sending Device Metrics: ") 
                      << getId() 
                      << endl;
    Homie.getLogger() << cIndent
                      << F("RSSI: ")
                      << WiFi.RSSI()
                      << endl;
    Homie.getLogger() << cIndent
                      << F(" MAC: ")
                      << WiFi.macAddress().c_str()
                      << endl;
    Homie.getLogger() << cIndent
                      << F(" Reset Reason: ")
                      << esp_reset_reason()
                      << endl;

    setProperty(cPropertySignal)
        .setRetained(true)
        .send(String(WiFi.RSSI()));

    setProperty(cPropertyMac)
        .setRetained(true)
        .send(WiFi.macAddress());

    setProperty(cPropertyResetReason)
        .setRetained(true)
        .send(String(esp_reset_reason()));
  }
}
