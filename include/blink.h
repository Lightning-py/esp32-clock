#pragma once

#include <Arduino.h>

#include "info.h"

void Blink(void *pvParameters) {
    bool status = false;

    while (1) {
        status = !status;
        digitalWrite(LED_PIN, status);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}