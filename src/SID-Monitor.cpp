#include "application.h"
#line 1 "/Users/elee/Projects/SaintGimp/SID-Monitor/src/SID-Monitor.ino"
void setup();
void loop();
#line 1 "/Users/elee/Projects/SaintGimp/SID-Monitor/src/SID-Monitor.ino"
const double vref = 3.328;

int adcValue;
double voltageLevel;
double exponentialAverage = -1;
// alpha is 2/(N+1), which results in the last N samples being ~86% of the weight
double alpha = 2.0 / (10 + 1);

unsigned long lastPollTime = 0;
unsigned long lastMinuteTime = 0;

void setup() {
    setADCSampleTime(ADC_SampleTime_480Cycles);
    pinMode(D7, OUTPUT);
}

void loop() {
    // Sleep however long we need to in order to get to the next two-second mark.
    // If unsigned math overflows, just sleep for two seconds.
    unsigned long timeToSleep = (lastPollTime + 2000) - millis();
    if (timeToSleep > 2000)
    {
        timeToSleep = 2000;
    }
    delay(timeToSleep);
    lastPollTime = millis();

    // Flash the blue LED for status
    digitalWrite(D7, HIGH);
    delay(10);
    digitalWrite(D7, LOW);

    // Read the ADC, convert to voltage, and update running 
    adcValue = analogRead(A0);
    voltageLevel = adcValue * vref / 4095;
    if (exponentialAverage < 0)
    {
        exponentialAverage = voltageLevel;
    }
    else
    {
        exponentialAverage += (voltageLevel - exponentialAverage) * alpha;
    }
    
    // Publish to the fast stream
    // Remember to create a webhook to forward data to storage
    String fastData = "";
    fastData = fastData + "{\"voltage\":" + voltageLevel + "}";
    Particle.publish("sid-fast", fastData, 60, PRIVATE);

    if (millis() - lastMinuteTime >= 60000)
    {
        lastMinuteTime = millis();

        String slowData = "";
        slowData = slowData + "{\"voltage\":" + exponentialAverage + "}";
        Particle.publish("sid-monitor", slowData, 60, PRIVATE);
    }
}