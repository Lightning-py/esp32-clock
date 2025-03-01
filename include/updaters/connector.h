#pragma once

#include <Arduino.h>
#include <WiFi.h>

#include "info.h"
#include "timeUpdater.h"

void connect(void* deskriptor) {
    WiFi.mode(WIFI_STA);

    WiFi.begin(ssid, password);

#ifdef DEBUG
    int counter = 0;
#endif  // DEBUG
    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(100 / portTICK_PERIOD_MS);

#ifdef DEBUG
        if (counter == 20) {
            Serial.print("\nConnecting to Wifi..");
            counter = 0;
        } else {
            Serial.print(".");
        }
#endif  // DEBUG
    }

#ifdef DEBUG
    Serial.println("Connected to WiFi!");
    Serial.print("IP Address: ");
    Serial.print(WiFi.localIP());
#endif  // DEBUG
    vTaskDelete(NULL);
}
