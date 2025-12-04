#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <timers.h>

#define TRIG_PIN 11
#define ECHO_PIN 12

#define BUZZER_PIN 13

#define SENSOR_TASK_STACK_SIZE 64
#define SENSOR_TASK_PRIORITY 1

#define BUZZER_TASK_STACK_SIZE 64
#define BUZZER_TASK_PRIORITY 1

#define HALT_LOWER_BOUND 2.0
#define HALT_UPPER_BOUND 5.0

#define DANGER_LOWER_BOUND 5.0
#define DANGER_UPPER_BOUND 10.0

#define CAUTION_LOWER_BOUND 10.0
#define CAUTION_UPPER_BOUND 15.0

#define SAFE_LOWER_BOUND 15.0

#define DANGER_BUZZER_PERIOD 250 / portTICK_PERIOD_MS
#define CAUTION_BUZZER_PERIOD 500 / portTICK_PERIOD_MS

enum parking_state { HALT, DANGER, CAUTION, SAFE };

volatile parking_state current_state = SAFE;

SemaphoreHandle_t xMutex = NULL;

TaskHandle_t xSensorTaskHandle = NULL;
TaskHandle_t xBuzzerTaskHandle = NULL;

TimerHandle_t xTimerHandle = NULL;

volatile bool is_buzzer_on = false;

inline void terminate_program() {
    if (xMutex != NULL) {
        vSemaphoreDelete(xMutex);
    }
    if (xSensorTaskHandle != NULL) {
        vTaskDelete(xSensorTaskHandle);
    }
    if (xBuzzerTaskHandle != NULL) {
        vTaskDelete(xBuzzerTaskHandle);
    }
    if (xTimerHandle != NULL) {
        while (xTimerDelete(xTimerHandle, 0) == pdFAIL)
            ;
    }
}

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
                xTaskNotifyGive(xBuzzerTaskHandle);
            }
        }
    }
}

void vTimerCallback(TimerHandle_t) {
    if (is_buzzer_on) {
        digitalWrite(BUZZER_PIN, LOW);
    } else {
        digitalWrite(BUZZER_PIN, HIGH);
    }
    is_buzzer_on = !is_buzzer_on;
}

inline void timer_off() {
    if (xTimerIsTimerActive(xTimerHandle) != pdFALSE) {
        if (xTimerDelete(xTimerHandle, 0) == pdFAIL) {
            Serial.println("Non-fatal Error: failed to stop timer");
        }
    }
}

inline void change_buzzer_period(parking_state new_state) {
    static const char *error_msg =
        "Non-fatal Error: failed to change timer period";
    switch (new_state) {
        case HALT:
            digitalWrite(BUZZER_PIN, HIGH);
            is_buzzer_on = true;
            break;
        case DANGER:
            if (xTimerChangePeriod(xTimerHandle, DANGER_BUZZER_PERIOD, 0) ==
                pdFAIL) {
                Serial.println(error_msg);
            }
            break;
        case CAUTION:
            if (xTimerChangePeriod(xTimerHandle, CAUTION_BUZZER_PERIOD, 0) ==
                pdFAIL) {
                Serial.println(error_msg);
            }
            break;
        case SAFE:
            digitalWrite(BUZZER_PIN, LOW);
            is_buzzer_on = false;
            break;
    }
}

void buzzer_task(void *) {
    pinMode(BUZZER_PIN, OUTPUT);
    const TickType_t xTicksToWait = 10;
    parking_state new_state;
    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (xSemaphoreTake(xMutex, xTicksToWait) == pdTRUE) {
            new_state = current_state;
            xSemaphoreGive(xMutex);
            timer_off();
            change_buzzer_period(new_state);
        }
    }
}

void setup() {
    Serial.begin(9600);
    xMutex = xSemaphoreCreateMutex();
    if (xMutex == NULL) {
        Serial.println("Fatal Error: failed to create mutex");
        return;
    }
    xTimerHandle = xTimerCreate("timer", 1, pdTRUE, NULL, vTimerCallback);
    if (xTimerHandle == NULL) {
        Serial.println("Fatal Error: failed to create timer");
        terminate_program();
        return;
    }
    if (xTaskCreate(sensor_task, "sensor task", SENSOR_TASK_STACK_SIZE, NULL,
                    SENSOR_TASK_PRIORITY, &xSensorTaskHandle) != pdPASS) {
        Serial.println("Fatal Error: failed to create sensor task");
        terminate_program();
        return;
    }
    if (xTaskCreate(buzzer_task, "buzzer task", BUZZER_TASK_STACK_SIZE, NULL,
                    BUZZER_TASK_PRIORITY, &xBuzzerTaskHandle) != pdPASS) {
        Serial.println("Fatal Error: failed to create buzzer task");
        terminate_program();
        return;
    }
}

void loop() {}
