#include <DHT.h>

// configuration of the buzzer
#define BUZZER_PIN 5
#define BUZZER_FREQ 100

// configuration of the dht sensor
#define DHT_PIN 7
#define DHT_TYPE DHT11
#define DHT_READ_DELAY 3000

// configure the thresholds when to start buzzing
#define HUMIDITY_THRESH_COEFF 1.1
#define TEMPERATURE_THRESH_COEFF 1.1

// object for interfacing with the sensor
DHT dht(DHT_PIN, DHT_TYPE);

// actual threshold values that are filled in the setup()
float threshold_humidity;
float threshold_temperature;

// true if alarm buzzing is done and the buzzer should be silent from now on
bool done = false;

void setup() {
    Serial.begin(9600);

    // make sure the sensor is initialized and read the normal/baseline humidity and temperature
    while (true) {
        dht.begin();
        delay(DHT_READ_DELAY);
        const float normal_humidity = dht.readHumidity();
        const float normal_temperature = dht.readTemperature();
        if (!isnan(normal_humidity) && !isnan(normal_temperature)) {
            threshold_humidity = normal_humidity * HUMIDITY_THRESH_COEFF;
            threshold_temperature = normal_temperature * TEMPERATURE_THRESH_COEFF;
            break;
        }
    }

    // buzz once to signal the start
    tone(BUZZER_PIN, BUZZER_FREQ);
    delay(250);
    noTone(BUZZER_PIN);

    // wait 3 minutes to make sure we're under the shower :)
    delay(180000);
}

void loop() {
    // are we already done? make sure the buzzer is off and sleep for 10 minutes in an endless loop.
    if (done) {
        noTone(BUZZER_PIN);
        delay(600000);
        return;
    }

    // read every 60 seconds
    delay(60000);
    const float current_humidity = dht.readHumidity();
    const float current_temperature = dht.readTemperature();
    if (isnan(current_humidity) || isnan(current_temperature)) {
        return;
    }

    // output for debugging
    Serial.print("Humidity: ");
    Serial.println(current_humidity);
    Serial.print("Temperature: ");
    Serial.println(current_temperature);

    // are the humidity and temperature again under the threshold?
    if (current_humidity < threshold_humidity && current_temperature < threshold_temperature) {
        // buzz for 3 minutes
        tone(BUZZER_PIN, BUZZER_FREQ);
        delay(180000);

        // deactive the buzzer
        noTone(BUZZER_PIN);

        // signal that we are done
        done = true;
    }
}
