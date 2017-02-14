# Mini Nextbus Display

This is ESP-8266 code that updates a small SSD-1306-based display with the next two predictions. It measures 1" x 1" and costs <$10USD.

<a href="https://www.youtube.com/watch?v=yCM8uwi17Rc" target="_blank"><img src="http://i.imgur.com/lzmuDaU.jpeg"
alt="Video" height="250" border="10" /><p>Demo Video</a>

## Hardware
* ESP8266 microcontroller, like a [Wemos D1 Mini](https://www.aliexpress.com/store/product/WEMOS-D1-mini-Pro-16M-bytes-external-antenna-connector-ESP8266-WIFI-Internet-of-Things-development-board/1331105_32724692514.html)
* SSD1036 OLED display, like a [Wemos OLED Shield](https://www.wemos.cc/product/oled-shield.html). You can stack this shield on top of the Wemos D1.
* Wifi connection

## Dependencies
* platformio
* ```pio lib install 13```

## Configuration
1. Change ```config.h``` to match your Wifi and MQTT settings
1. Go to nextbus.com, enter your stop information and change the other constants
1. Enter the time it takes to walk to your stop as MIN_WALKING_TIME. The display won't show any predictions less than this time.
