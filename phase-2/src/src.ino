#include <Arduino_FreeRTOS.h>
#include <semphr.h>

#define TRIG_PIN 11
#define ECHO_PIN 12

#define BUZZER_PIN LED_BUILTIN

#define SENSOR_TASK_STACK_SIZE 64
#define SENSOR_TASK_PRIORITY 1

#define BUZZER_TASK_STACK_SIZE 64
#define BUZZER_TASK_PRIORITY 1

#define HALT_LOWER_BOUND 0.0
#define HALT_UPPER_BOUND 5.0

#define DANGER_LOWER_BOUND 5.0
#define DANGER_UPPER_BOUND 10.0

#define CAUTION_LOWER_BOUND 10.0
#define CAUTION_UPPER_BOUND 15.0

#define SAFE_LOWER_BOUND 15.0

enum parking_state { HALT, DANGER, CAUTION, SAFE };

volatile parking_state current_state = SAFE;

SemaphoreHandle_t xMutex = NULL;

inline char *parking_state_to_string(parking_state s) {
    char *ret;
    switch (s) {
        case HALT:
            ret = "HALT";
            break;
        case DANGER:
            ret = "DANGER";
            break;
        case CAUTION:
            ret = "CAUTION";
            break;
        case SAFE:
            ret = "SAFE";
            break;
    }
    return ret;
}

inline bool is_in_bound(const float distance, const float lower_bound,
                        const float upper_bound) {
    if (lower_bound <= distance && distance < upper_bound) {
        return true;
    }
    return false;
}

inline parking_state distance_to_state(const float distance) {
    if (is_in_bound(distance, HALT_LOWER_BOUND, HALT_UPPER_BOUND)) {
        return HALT;
    } else if (is_in_bound(distance, DANGER_LOWER_BOUND, DANGER_UPPER_BOUND)) {
        return DANGER;
    } else if (is_in_bound(distance, CAUTION_LOWER_BOUND,
                           CAUTION_UPPER_BOUND)) {
        return CAUTION;
    } else {
        return SAFE;
    }
}

float get_distance_cm() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    unsigned long duration = pulseIn(ECHO_PIN, HIGH);
    return duration * 0.0343 / 2.0;
}

void sensor_task(void *) {
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    const TickType_t xTicksToWait = 10;
    for (;;) {
        float distance = get_distance_cm();
        parking_state new_state = distance_to_state(distance);
        if (new_state != current_state) {
            if (xSemaphoreTake(xMutex, xTicksToWait) == pdTRUE) {
                current_state = new_state;
                xSemaphoreGive(xMutex);
            }
        }
    }
}

void buzzer_task(void *) {
    pinMode(BUZZER_PIN, OUTPUT);
    const TickType_t xTicksToWait = 10;
    for (;;) {
        // if (xSemaphoreTake(xMutex, xTicksToWait) == pdTRUE) {
        //     xSemaphoreGive(xMutex);
        // }
    }
}

void setup() {
    Serial.begin(9600);
    xMutex = xSemaphoreCreateMutex();
    if (xMutex == NULL) {
        Serial.println("mutex creation failed");
        return;
    }
    TaskHandle_t xSensorTaskHandle = NULL;
    if (xTaskCreate(sensor_task, "sensor task", SENSOR_TASK_STACK_SIZE, NULL,
                    SENSOR_TASK_PRIORITY, &xSensorTaskHandle) != pdPASS) {
        Serial.println("sensor-task creation failed");
        vSemaphoreDelete(xMutex);
        return;
    }
    if (xTaskCreate(buzzer_task, "buzzer task", BUZZER_TASK_STACK_SIZE, NULL,
                    BUZZER_TASK_PRIORITY, NULL) != pdPASS) {
        Serial.println("buzzer-task creation failed");
        vSemaphoreDelete(xMutex);
        if (xSensorTaskHandle != NULL) {
            vTaskDelete(xSensorTaskHandle);
        }
        return;
    }
}

void loop() {}
