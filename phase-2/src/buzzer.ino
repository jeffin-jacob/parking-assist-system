void buzzer_task(void *) {
    for (;;) {
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            // critical section
            xSemaphoreGive(xMutex);
        }
    }
}
