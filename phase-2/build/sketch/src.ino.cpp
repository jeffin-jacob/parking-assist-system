#include <Arduino.h>
#line 1 "/Users/jeffinjacob/Documents/cse321/parking-assist-system/phase-2/src/src.ino"
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

#define BUZZER_STACK_SIZE 64 // words not bytes
#define BUZZER_PRIORITY 1

enum Distance { Safe, Caution, Danger, Stop };

volatile Distance system_state = Safe;
SemaphoreHandle_t xMutex = NULL;

#line 12 "/Users/jeffinjacob/Documents/cse321/parking-assist-system/phase-2/src/src.ino"
void setup();
#line 32 "/Users/jeffinjacob/Documents/cse321/parking-assist-system/phase-2/src/src.ino"
void loop();
#line 1 "/Users/jeffinjacob/Documents/cse321/parking-assist-system/phase-2/src/buzzer.ino"
void buzzer_task(void *);
#line 12 "/Users/jeffinjacob/Documents/cse321/parking-assist-system/phase-2/src/src.ino"
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

#line 1 "/Users/jeffinjacob/Documents/cse321/parking-assist-system/phase-2/src/buzzer.ino"
void buzzer_task(void *) {
    for (;;) {
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            // critical section
            xSemaphoreGive(xMutex);
        }
    }
}

