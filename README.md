# EnvironmentMonitor_DHT_
<a href="https://homieiot.github.io/">
  <img src="https://homieiot.github.io/img/works-with-homie.png" alt="works with MQTT Homie">
</a>

HomieNode V3 featuring:
- DHT Temperature Class, using HomeNode with auto detect of dht11/22.
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
sknSensors/LocationName/$state lost
sknSensors/LocationName/$state init
sknSensors/LocationName/$homie 3.0.1
sknSensors/LocationName/$name Environment Monitor
sknSensors/LocationName/$mac 38:2B:78:03:93:CB
sknSensors/LocationName/$localip 10.100.1.165
sknSensors/LocationName/$nodes ambient,presence
sknSensors/LocationName/$stats uptime
sknSensors/LocationName/$stats/interval 905
sknSensors/LocationName/$fw/name Environment-SHT31-RCWL0516
sknSensors/LocationName/$fw/version 1.0.3
sknSensors/LocationName/$fw/checksum dfcfb06b8338776bf6222441831af108
sknSensors/LocationName/$implementation esp8266
sknSensors/LocationName/$implementation/config {"name":"Environment Monitor","device_id":"LocationName","device_stats_interval":900,"wifi":{"ssid":"SFNSS1-24G"},"mqtt":{"host":"openhab.skoona.net","port":1883,"base_topic":"sknSensors/","auth":true},"ota":{"enabled":true},"settings":{"sensorInterval":900,"motionHoldInterval":60}}
sknSensors/LocationName/$implementation/version 3.0.0
sknSensors/LocationName/$implementation/ota/enabled true

sknSensors/LocationName/ambient/$name DHT Temperature and Humidity Sensor
sknSensors/LocationName/ambient/$type sensor
sknSensors/LocationName/ambient/$properties humidity,temperature
sknSensors/LocationName/ambient/humidity/$name Humidity
sknSensors/LocationName/ambient/humidity/$datatype float
sknSensors/LocationName/ambient/humidity/$unit %rH
sknSensors/LocationName/ambient/temperature/$name Temperature
sknSensors/LocationName/ambient/temperature/$datatype float
sknSensors/LocationName/ambient/temperature/$unit °F

sknSensors/LocationName/presence/$name RCWL-0516 Motion Sensor
sknSensors/LocationName/presence/$type sensor
sknSensors/LocationName/presence/$properties motion
sknSensors/LocationName/presence/motion/$name Motion
sknSensors/LocationName/presence/motion/$datatype enum
sknSensors/LocationName/presence/motion/$format OPEN,ClOSED

sknSensors/LocationName/$state ready
sknSensors/LocationName/ambient/temperature 56.98
sknSensors/LocationName/ambient/humidity 30.18
sknSensors/LocationName/presence/motion Open
sknSensors/LocationName/$stats/interval 905
sknSensors/LocationName/$stats/signal 70
sknSensors/LocationName/$stats/uptime 5
```