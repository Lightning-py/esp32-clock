#pragma once

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>

extern const char* url;
extern SemaphoreHandle_t WEATHER_DISPLAY_MUTEX;
typedef struct {
    double temp;
    int weather_code;
    bool is_data_updated;
} weather;

extern weather weather_data;

void weather_update(void* deskriptor) {
    while (1) {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        if (WiFi.status() != WL_CONNECTED) {
            continue;
        }

        HTTPClient http;
        http.begin(url);
        int httpResponseCode = http.GET();  // Выполнение GET-запроса

        // Serial.printf("\nGot http response: %d\n", httpResponseCode);

        if (httpResponseCode > 0) {
            String payload = http.getString();  // Получение ответа
            // Serial.println("Ответ от сервера:");
            // Serial.println(payload);  // Вывод ответа на серийный порт

            JsonDocument doc;  // Создание документа JSON

            DeserializationError error = deserializeJson(doc, payload);

            if (error) {
                Serial.print("Ошибка декодирования JSON: ");

                Serial.println(error.f_str());

                return;
            }

            JsonObject current = doc["current"];
            double temp = current["temperature_2m"];
            int weather_code = current["weather_code"];

            xSemaphoreTake(WEATHER_DISPLAY_MUTEX, portTICK_PERIOD_MS);
            weather_data.temp = temp;
            weather_data.weather_code = weather_code;
            weather_data.is_data_updated = true;
            xSemaphoreGive(WEATHER_DISPLAY_MUTEX);
        }

        http.end();
    }
}