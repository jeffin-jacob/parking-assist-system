#include <Arduino_FreeRTOS.h>
#include <semphr.h>

#define BUZZER_STACK_SIZE 64 // words not bytes
#define BUZZER_PRIORITY 1

enum Distance { Safe, Caution, Danger, Stop };

volatile Distance system_state = Safe;
SemaphoreHandle_t xMutex = NULL;

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        // wait for serial port to initialize
    }
    xMutex = xSemaphoreCreateMutex();
    if (xMutex == NULL) {
        Serial.println("mutex creation failed");
        return;
    }
    BaseType_t xReturn;
    xReturn = xTaskCreate(buzzer_task, "buzzer task", BUZZER_STACK_SIZE, NULL,
                          BUZZER_PRIORITY, NULL);
    if (xReturn != pdPASS) {
        Serial.println("task creation failed");
        vSemaphoreDelete(xMutex);
        return;
    }
}

void loop() {}
