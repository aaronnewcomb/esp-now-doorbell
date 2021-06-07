# esp-now-doorbell

This doorbell system uses ESP-NOW with a mixture of ESP8266 and ESP32 devices to allow the user to create their own doorbell system which plays whatever kind of sound file they choose. The doorbell receiver component uses a DFPlayer MP3 player module. IOT_button_now_ESP32 could be used with an intercom box to optionally also play a sound out of the intercom box speaker.

## Video

## Files
Doorbell_now_ESP8266.ino - The doorbell receiver meant to run on an ESP8266 with DFPlayer module.
IoT_button_now_ESP8266 - The doorbell sender script meant to run on an ESP8266.
IoT_button_now_ESP32 - The doorbell sender script meant to be run on an ESP32 with an optional DFPlayer module.
