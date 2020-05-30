# Shower Alert

This project uses an Arduino Micro with a DHT sensor and 6V buzzer attached. The use case is to place the Arduino in the bathroom and start it directly before going into the shower. It will then read the baseline humidity and temperature. While showering the humidity and temperature in the room will increase. They will decrease again after showering while the ventilation of the bathroom is active. The Arduino should recognize when the humidity is low enough again and start buzzing to make sure you don't forget to turn of the ventilation.

Libraries used:
- Adafruit DHT

Hardware layout:
- PIN 5: 6V buzzer
- PIN 7: DHT-11 sensor.
