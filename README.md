# sensorNode

MQTT Sensor Node for [Home Assistant](https://home-assistant.io/)

* [BRUH AUtomation](https://www.youtube.com/watch?v=jpjfVc-9IrQ)
* [ESPURNA](https://bitbucket.org/xoseperez/espurna/src/e6e9f9d6d4bd?at=master)
* [ESPURNA-Board](https://github.com/xoseperez/espurna-board)

# GOAL

Sensors for every room for Home Assistant that fits into an outlet with a Webinterface to configurate Wifi, Mqtt and the Sensors

# Process

## Sensors

- [x] Temperature
- [x] Humidity
- [ ] Motion - waiting for delivery
- [x] Light

## LED

- [ ] MQTT controlled RGB LED - waiting for delivery

## WiFi

- [x] WiFiManager: Connect from saved values. If no connection possible -> captive gateway

## MQTT 

- [x] Sending Sensor values - check
- [ ] Start MQTT only if Settings are set
- [ ] Username und Passwort -> optional
- [ ] Recieving commands for the LED

## Webserver

- [x] Style
- [x] Changing Hostname
- [x] MQTT Settings
- [ ] No restart after saving

## OTA

* Sometimes it works...any ideas?

## Settings

- [x] Save Hostname permanently
- [x] Save MQTT Settings permanently
- [ ] Available Sensors

## Board

* Like the ESPURNA-Board without the Relay ( and POW?) but with header pins for the GPIO Pins of the esp12 to connect the sensors

## Enclosure

* Similar to an in-wall switch or motionsensor 3D printed
