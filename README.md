# EnvironmentMonitor_DHT
<a href="https://homieiot.github.io/">
  <img src="https://homieiot.github.io/img/works-with-homie.png" alt="works with MQTT Homie">
</a>

HomieNode V3 featuring:
- DHT Temperature Class, using HomeNode with auto detect sensor of DHT11, DHT22, AM2302, RHT03.
- RCWL-0516 Doppler Radar Microwave Motion Class, using HomieNode.

### Homie Config

```
{
  "name": "Environment Monitor",
  "device_id": "<device-location-name>",
  "device_stats_interval": 900,  
  "wifi": {
    "ssid": "<wifi-host>",
    "password": "<wifi-password>"
  },
  "mqtt": {
    "host": "<mqtt-hostname-or-ip>",
    "port": 1883,
	"base_topic": "sknSensors/",
    "auth": true,
    "username": "<mqtt-username>",
    "password": "<mqtt-password>"
  },
  "ota": {
    "enabled": true
  },
  "settings": {
      "sensorInterval": 900,
      "motionHoldInterval": 60
    }
}
```


### Home MQTT Log
```
sknSensors/HomeOffice/$state lost
sknSensors/HomeOffice/$state init
sknSensors/HomeOffice/$homie 3.0.1
sknSensors/HomeOffice/$name Home Office
sknSensors/HomeOffice/$mac 84:F3:EB:B7:55:D5
sknSensors/HomeOffice/$localip 10.100.1.161
sknSensors/HomeOffice/$nodes Ambient,Presence
sknSensors/HomeOffice/$stats uptime
sknSensors/HomeOffice/$stats/interval 905
sknSensors/HomeOffice/$fw/name Environment-DHT-RCWL0516
sknSensors/HomeOffice/$fw/version 1.0.0
sknSensors/HomeOffice/$fw/checksum 66625e09720864223f16f3dec17b570d
sknSensors/HomeOffice/$implementation esp8266
sknSensors/HomeOffice/$implementation/config {"name":"Home Office","device_id":"HomeOffice","device_stats_interval":900,"wifi":{"ssid":"SFNSS1-24G"},"mqtt":{"host":"openhab.skoona.net","port":1883,"base_topic":"sknSensors/","auth":true},"ota":{"enabled":true},"settings":{"sensorsInterval":900}}
sknSensors/HomeOffice/$implementation/version 3.0.0
sknSensors/HomeOffice/$implementation/ota/enabled true
sknSensors/HomeOffice/Ambient/$name DHT Temperature and Humidity Sensor
sknSensors/HomeOffice/Ambient/$type sensor
sknSensors/HomeOffice/Ambient/$properties humidity,temperature
sknSensors/HomeOffice/Ambient/humidity/$name Humidity
sknSensors/HomeOffice/Ambient/humidity/$datatype float
sknSensors/HomeOffice/Ambient/humidity/$unit %rH
sknSensors/HomeOffice/Ambient/temperature/$name Temperature
sknSensors/HomeOffice/Ambient/temperature/$datatype float
sknSensors/HomeOffice/Ambient/temperature/$unit °F
sknSensors/HomeOffice/Presence/$name RCWL-0516 Motion Sensor
sknSensors/HomeOffice/Presence/$type sensor
sknSensors/HomeOffice/Presence/$properties motion
sknSensors/HomeOffice/Presence/motion/$name Motion
sknSensors/HomeOffice/Presence/motion/$datatype enum
sknSensors/HomeOffice/Presence/motion/$format OPEN,CLOSED
sknSensors/HomeOffice/$state ready
sknSensors/HomeOffice/Ambient/temperature 77.18
sknSensors/HomeOffice/Ambient/humidity 29.00
sknSensors/HomeOffice/$stats/interval 905
sknSensors/HomeOffice/$stats/signal 64
sknSensors/HomeOffice/$stats/uptime 6



```