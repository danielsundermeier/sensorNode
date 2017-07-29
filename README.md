# sensorNode

MQTT Sensor Node for [Home Assistant](https://home-assistant.io/)

* [BRUH AUtomation](https://www.youtube.com/watch?v=jpjfVc-9IrQ)
* [ESPURNA](https://bitbucket.org/xoseperez/espurna/src/e6e9f9d6d4bd?at=master)
* [ESPURNA-Board](https://github.com/xoseperez/espurna-board)

# GOAL

Sensors for every room for Home Assistant that fits into an outlet with a Webinterface to configurate Wifi, Mqtt and the Sensors

# Process

## Sensors

* Temperature - check
* Humidity - check
* Motion - waiting for delivery
* Light - check

## LED

* MQTT controlled RGB LED

## MQTT 

* Sending Sensor values - check
* Recieving commands for the LED

## Webserver

* changing Hostname - check 
* mDNS nor working (Hostname.local)
* Style

## OTA

* Does not work correctly

## Settings

* save Hostname permanently - check
* save other settings
* Multiple Wifi Networks
* MQTT IP, User, Password, Command Topic, State Topic
* Available Sensors

## Board

* Like the ESPURNA-Board without the Relay ( and POW?) but with header pins for the GPIO Pins of the esp12 to connect the sensors

## Enclosure

* Similar to an in-wall switch or motionsensor 3D printed
