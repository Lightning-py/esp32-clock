#pragma once

#include <Arduino.h>

#include "effects/effects.h"
#include "info.h"
#include "led_matrix/led.h"

extern LedMatrix matrix;

int handler(std::string command) {
    if (command.size() < 1) command += "\t";

    if (command[command.size() - 1] == '\n')
        command = command.substr(0, command.length() - 1);

    if (command == "turn_on") {
        vTaskResume(LedRunHandler);

        return 1;
    } else if (command == "turn_off") {
        vTaskSuspend(LedRunHandler);

        pixels.clear();
        pixels.show();

        return 1;
    } else if (command == "stripe") {
        vTaskSuspend(LedRunHandler);

        xTaskCreate(stripe,    //
                    "Stripe",  //
                    2048,      //
                    NULL,      //
                    1,         //
                    &STRIPE    //
        );

        return 1;
    } else if (command == "snake") {
        vTaskSuspend(LedRunHandler);

        xTaskCreate(snake,    //
                    "Snake",  //
                    2048,     //
                    NULL,     //
                    1,        //
                    &SNAKE    //
        );

        return 1;
    } else if (command == "remaining_stripe") {
        vTaskSuspend(LedRunHandler);

        xTaskCreate(remaining_stripe,    //
                    "Remaining_stripe",  //
                    2048,                //
                    NULL,                //
                    1,                   //
                    &SNAKE               //
        );

        return 1;
    }

    Serial.printf("unknown command\n");
    return -1;
}
