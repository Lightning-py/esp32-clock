#pragma once

#include <Arduino.h>
#include <WiFi.h>

#include "timeUpdater.h"

extern const char* ssid;
extern const char* password;

void connect(void* deskriptor) {
    WiFi.mode(WIFI_STA);

    WiFi.begin(ssid, password);

    int counter = 0;

    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(100 / portTICK_PERIOD_MS);

        if (counter == 20) {
            Serial.print("\nConnecting to Wifi..");
            counter = 0;
        } else {
            Serial.print(".");
        }
    }

    Serial.println("Connected to WiFi!");
    Serial.print("IP Address: ");
    Serial.print(WiFi.localIP());

    // while (1) {
    //     Serial.print(WiFi.status());
    //     vTaskDelay(2000 / portTICK_PERIOD_MS);
    // }

    vTaskDelete(NULL);
}
