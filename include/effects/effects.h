#pragma once

#include <Arduino.h>

#include "../led_matrix/led.h"

TaskHandle_t SNAKE = NULL;
TaskHandle_t STRIPE = NULL;
TaskHandle_t REMAINING_STRIPE = NULL;

extern LedMatrix matrix;

void snake(void* deskriptor) {
    matrix.clear();

    for (int j = 0; j < 256 - 8; ++j) {
        matrix.clear();

        for (int i = 0; i < 8; ++i) {
            int x = (j + i) / matrix.getLedY();
            int y = (j + i) % matrix.getLedY();
            matrix.setColor(x, y, 10, 0, 0);
        }

        matrix.update();

        vTaskDelay(5 / portTICK_PERIOD_MS);
    }

    vTaskResume(LedRunHandler);
    vTaskDelete(NULL);
}

void remaining_stripe(void* deskriptor) {
    for (int i = 0; i < matrix.getLedX(); ++i) {
        for (int j = 0; j < matrix.getLedY(); ++j) {
            matrix.setColor(i, j, 0, 10, 10);
        }

        matrix.update();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    vTaskResume(LedRunHandler);
    vTaskDelete(NULL);
}

void stripe(void* desktiptor) {
    for (int i = 0; i < matrix.getLedX(); ++i) {
        matrix.clear();

        for (int j = 0; j < matrix.getLedY(); ++j) {
            matrix.setColor(i, j, 0, 10, 10);
        }

        matrix.update();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    vTaskResume(LedRunHandler);
    vTaskDelete(NULL);
}