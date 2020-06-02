#include <DHT.h>
#include "LowPower.h"

// set this to print the values to serial for monitoring
// #define DO_MONITORING

// set this to not use low power states in order to use the serial communication
// #define DO_DEBUGGING

// configuration of the buzzer
#define BUZZER_PIN 5
#define BUZZER_FREQ 100

// configuration of the dht sensor
#define DHT_PIN 7
#define DHT_TYPE DHT11
#define DHT_READ_DELAY_INIT 3000
#define DHT_READ_DELAY_WORK 60000

// object for interfacing with the sensor
DHT dht(DHT_PIN, DHT_TYPE);

// initial and maximum humidity
float initial_humidity;
float maximal_humidity;

// true if alarm buzzing is done and the buzzer should be silent from now on
bool done = false;

#if 0
void delay_low_power(unsigned long milliseconds) {
#if defined(DO_MONITORING) || defined(DO_DEBUGGING)
    delay(milliseconds);
#else
    const unsigned long seconds = milliseconds / 1000;
    if (seconds < 4) {
        for (unsigned long i = 0; i < seconds; ++i) {
            LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
        }
    } else {
        const unsigned long num_cycles = seconds / 4;
        for (unsigned long i = 0; i < seconds; ++i) {
            LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
        }
    }
#endif
}
#endif

void setup() {
    Serial.begin(9600);

    // WARNING: do not use delay_low_power() here and give the user some time to e.g. upload sketches.
    //          uploading new sketches won't be able while in delay_low_power()!!!

    // make sure the sensor is initialized and read the normal/baseline humidity and temperature
    Serial.println("# Doing initial readings.");
    dht.begin();
    while (true) {
        delay(DHT_READ_DELAY_INIT);
        const float current_humidity = dht.readHumidity();
        const float current_temperature = dht.readTemperature();
        Serial.print(current_humidity);
        Serial.print('\t');
        Serial.println(current_temperature);
        if (!isnan(current_humidity) && !isnan(current_temperature)) {
            initial_humidity = current_humidity;
            maximal_humidity = current_humidity;
            break;
        }
    }

    // buzz once to signal the start
    tone(BUZZER_PIN, BUZZER_FREQ);
    delay(250);
    noTone(BUZZER_PIN);

    // wait for 3 minutes to make sure we are under the shower :)
#if !defined(DO_MONITORING)
    Serial.println("# Waiting 3 minutes.");
    delay(180000);
    Serial.println("# Startup finished.");
#endif
}

void loop() {
    // do monitoring or actual use-case work for shower alert?
#if defined(DO_MONITORING)
    delay(DHT_READ_DELAY_WORK);
    const float current_humidity = dht.readHumidity();
    const float current_temperature = dht.readTemperature();
    Serial.print(current_humidity);
    Serial.print('\t');
    Serial.println(current_temperature);
#else
    // are we already done? make sure the buzzer is off and sleep for 10 minutes in an endless loop.
    if (done) {
        noTone(BUZZER_PIN);
        delay(600000);
        return;
    }

    // read
    delay(DHT_READ_DELAY_WORK);
    Serial.println("# Reading values.");
    dht.read();
    const float current_humidity = dht.readHumidity();
    const float current_temperature = dht.readTemperature();
    if (isnan(current_humidity) || isnan(current_temperature)) {
        return;
    }

    // output for debugging
    Serial.print(current_humidity);
    Serial.print('\t');
    Serial.println(current_temperature);

    // store new maximal humidity or check if we are low enough again
    if (current_humidity > maximal_humidity) {
        maximal_humidity = current_humidity;
    } else {
        const float delta_humidity = maximal_humidity - initial_humidity;
        const float threshold_humidity = initial_humidity + (delta_humidity * 0.5);
        if (delta_humidity > 20.0 && current_humidity < threshold_humidity) {
            // buzz for 3 minutes
            tone(BUZZER_PIN, BUZZER_FREQ);
            delay(180000);

            // deactive the buzzer
            noTone(BUZZER_PIN);

            // signal that we are done
            done = true;
        }
    }
#endif
}
