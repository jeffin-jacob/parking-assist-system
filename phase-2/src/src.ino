#include <Arduino_FreeRTOS.h>
#include <semphr.h>

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
        // process distance
        vTaskDelay(xDelay);
    }
}

void setup() {
    Serial.begin(9600);
    if (xTaskCreate(sensor_task, "sensor task", 64, NULL, 1, NULL) != pdPASS) {
        Serial.println("sensor-task creation failed");
    }
}

void loop() {}
