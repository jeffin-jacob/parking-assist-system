#include <Arduino.h>
#line 1 "/Users/jeffinjacob/Documents/cse321/parking-assist-system/phase-2/src/src.ino"
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

#line 4 "/Users/jeffinjacob/Documents/cse321/parking-assist-system/phase-2/src/src.ino"
void sensor_task(void *);
#line 25 "/Users/jeffinjacob/Documents/cse321/parking-assist-system/phase-2/src/src.ino"
void setup();
#line 34 "/Users/jeffinjacob/Documents/cse321/parking-assist-system/phase-2/src/src.ino"
void loop();
#line 4 "/Users/jeffinjacob/Documents/cse321/parking-assist-system/phase-2/src/src.ino"
void sensor_task(void *) {
    const int trig_pin = 11, echo_pin = 12;
    pinMode(trig_pin, OUTPUT);
    pinMode(echo_pin, INPUT);
    float duration = 0.0, distance = 0.0;
    const TickType_t xDelay = 100 / portTICK_PERIOD_MS;
    for (;;) {
        digitalWrite(trig_pin, LOW);
        delayMicroseconds(2);
        digitalWrite(trig_pin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trig_pin, LOW);
        duration = pulseIn(echo_pin, HIGH);
        distance = (duration * 0.0343) / 2.0;
        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.println(" cm");
        vTaskDelay(xDelay);
    }
}

void setup() {
    Serial.begin(9600);
    BaseType_t xReturn;
    xReturn = xTaskCreate(sensor_task, "Sensor Task", 64, NULL, 1, NULL);
    if (xReturn != pdPASS) {
        Serial.println("task creation failed");
    }
}

void loop() {}

