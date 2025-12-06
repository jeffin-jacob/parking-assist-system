#include <Arduino_FreeRTOS.h>
#include <semphr.h>

#define SENSOR_TASK_STACK_SIZE 64
#define SENSOR_TASK_PRIORITY 1

#define TEST_TASK_STACK_SIZE 64
#define TEST_TASK_PRIORITY 1

#define HALT_LOWER_BOUND 2.0
#define HALT_UPPER_BOUND 5.0

#define DANGER_LOWER_BOUND 5.0
#define DANGER_UPPER_BOUND 10.0

#define CAUTION_LOWER_BOUND 10.0
#define CAUTION_UPPER_BOUND 15.0

#define SAFE_LOWER_BOUND 15.0

enum parking_state { HALT, DANGER, CAUTION, SAFE };

parking_state current_state = SAFE;

SemaphoreHandle_t xMutex = NULL;

inline bool is_in_bound(const float distance, const float lower_bound,
                        const float upper_bound) {
    if (lower_bound <= distance && distance < upper_bound) {
        return true;
    }
    return false;
}

inline parking_state distance_to_state(const float distance) {
    if (distance < HALT_LOWER_BOUND) {
        return current_state;
    } else if (is_in_bound(distance, HALT_LOWER_BOUND, HALT_UPPER_BOUND)) {
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

float get_distance_cm(const int trig_pin, const int echo_pin) {
    digitalWrite(trig_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(trig_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_pin, LOW);
    unsigned long duration = pulseIn(echo_pin, HIGH);
    return duration * 0.0343 / 2.0;
}

void sensor_task(void *) {
    const int trig_pin = 11, echo_pin = 12;
    pinMode(trig_pin, OUTPUT);
    pinMode(echo_pin, INPUT);
    const TickType_t xTicksToWait = 10;
    for (;;) {
        float distance = get_distance_cm(trig_pin, echo_pin);
        parking_state new_state = distance_to_state(distance);
        if (new_state != current_state) {
            if (xSemaphoreTake(xMutex, xTicksToWait) == pdTRUE) {
                current_state = new_state;
                xSemaphoreGive(xMutex);
            }
        }
    }
}

inline void test_prompt(const float lower_bound) {
    Serial.print("Place obstacle past ");
    Serial.print(lower_bound);
    Serial.print(" cm... ");
}

inline void test_prompt(const float lower_bound, const float upper_bound) {
    Serial.print("Place obstacle between ");
    Serial.print(lower_bound);
    Serial.print(" cm and ");
    Serial.print(upper_bound);
    Serial.print(" cm... ");
}

inline void cycle_tests(parking_state expected) {
    switch (expected) {
        case HALT:
            test_prompt(HALT_LOWER_BOUND, HALT_UPPER_BOUND);
            break;
        case DANGER:
            test_prompt(DANGER_LOWER_BOUND, DANGER_UPPER_BOUND);
            break;
        case CAUTION:
            test_prompt(CAUTION_LOWER_BOUND, CAUTION_UPPER_BOUND);
            break;
        case SAFE:
            test_prompt(SAFE_LOWER_BOUND);
            break;
    }
}

void test_task(void *) {
    const TickType_t xDelay = 5000 / portTICK_PERIOD_MS;
    Serial.println("Begin Test");
    for (parking_state expected = HALT;;
         expected = (expected + 1) % (SAFE + 1)) {
        cycle_tests(expected);
        vTaskDelay(xDelay);
        parking_state actual;
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            actual = current_state;
            xSemaphoreGive(xMutex);
        }
        if (actual == expected) {
            Serial.println("Pass");
        } else {
            Serial.println("Fail");
        }
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
    if (xTaskCreate(test_task, "test task", TEST_TASK_STACK_SIZE, NULL,
                    TEST_TASK_PRIORITY, NULL) != pdPASS) {
        Serial.println("test-task creation failed");
        vSemaphoreDelete(xMutex);
        if (xSensorTaskHandle != NULL) {
            vTaskDelete(xSensorTaskHandle);
        }
        return;
    }
}

void loop() {}
