#include <Arduino.h>
#include <Ds1302.h>
#include <HTTPClient.h>
#include <WiFi.h>

#include "blink.h"
#include "connector.h"
#include "info.h"  // файл с конфигурационными данными, в том числе паролем WiFi, url с локацией и тому подобной информации
#include "time.h"
#include "timeUpdater.h"
#include "weatherUpdater.h"

Ds1302 rtc(PIN_ENA, PIN_CLK, PIN_DAT);  // объект для работы с часами Ds1302

weather weather_data = {0.0, -1, false};  // объект для хранения данных погоды

Ds1302::DateTime TIME_DISPLAY_VAR = {
    .year = 0,
    .month = Ds1302::MONTH_JAN,
    .day = 0,
    .hour = 0,
    .minute = 0,
    .second = 0,
    .dow = Ds1302::DOW_SUN};  // объект для хранения данных времени

SemaphoreHandle_t
    TIME_DISPLAY_MUTEX;  // мьютекс для доступа к переменной времени
SemaphoreHandle_t WEATHER_DISPLAY_MUTEX;  // --  к переменной погоды

TaskHandle_t Task1Handle = NULL;          // Дескрипторы задач
TaskHandle_t BlinkHandle = NULL;          //
TaskHandle_t ConnectorHandle = NULL;      //
TaskHandle_t TimeDisplayHandle = NULL;    //
TaskHandle_t WeatherUpdateHandle = NULL;  //

#define TIME_UPDATE_STACK_SIZE \
    2048  // Размер стека задачи по обновлению времени
#define BLINK_STACK_SIZE 1024  // -- по свечению
#define CONNECTOR_STACK_SIZE 2048  // -- по соединению с интернетом
#define WEATHER_UPDATE_STACK_SIZE 4096  // -- по обновлению погоды

void time_display(void *deskriptor) {
    while (1) {
        xSemaphoreTake(TIME_DISPLAY_MUTEX, portMAX_DELAY);

        static uint8_t last_second = 0;
        Serial.print("20");
        Serial.print(TIME_DISPLAY_VAR.year);  // 00-99
        Serial.print('-');
        if (TIME_DISPLAY_VAR.month < 10) Serial.print('0');
        Serial.print(TIME_DISPLAY_VAR.month);  // 01-12
        Serial.print('-');
        if (TIME_DISPLAY_VAR.day < 10) Serial.print('0');
        Serial.print(TIME_DISPLAY_VAR.day);  // 01-31
        Serial.print("\t");
        if (TIME_DISPLAY_VAR.hour < 10) Serial.print('0');
        Serial.print(TIME_DISPLAY_VAR.hour);  // 00-23
        Serial.print(':');
        if (TIME_DISPLAY_VAR.minute < 10) Serial.print('0');
        Serial.print(TIME_DISPLAY_VAR.minute);  // 00-59
        Serial.print(':');
        if (TIME_DISPLAY_VAR.second < 10) Serial.print('0');
        Serial.print(TIME_DISPLAY_VAR.second);  // 00-59

        xSemaphoreGive(TIME_DISPLAY_MUTEX);

        xSemaphoreTake(WEATHER_DISPLAY_MUTEX, portMAX_DELAY);

        Serial.printf("\t Temperature: %f, weather_code: %d\n",
                      weather_data.temp, weather_data.weather_code);

        xSemaphoreGive(WEATHER_DISPLAY_MUTEX);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(9600);

    rtc.init();  // инициализация RTC

    pinMode(LED_PIN, OUTPUT);  // бинд порта светодиода на вывод

    TIME_DISPLAY_MUTEX =
        xSemaphoreCreateMutex();  // создание мьютекса для обновления времени
    WEATHER_DISPLAY_MUTEX = xSemaphoreCreateMutex();  // -- погоды

    WiFi.onEvent(
        updateTime,
        ARDUINO_EVENT_WIFI_STA_CONNECTED);  // обновление времени по NTP после
                                            // подключения к WiFi

    xTaskCreate(Blink, "Blink", BLINK_STACK_SIZE, NULL, 1, &BlinkHandle);
    xTaskCreate(connect, "Wifi", CONNECTOR_STACK_SIZE, NULL, 1,
                &ConnectorHandle);
    xTaskCreate(Continious_time_updater, "Time update", TIME_UPDATE_STACK_SIZE,
                NULL, 1, &Task1Handle);
    xTaskCreate(weather_update, "Weather update", WEATHER_UPDATE_STACK_SIZE,
                NULL, 1, &WeatherUpdateHandle);

    // --------------------------------------------------------------------------------------------------------------------------------

    xTaskCreate(
        time_display, "Time display", 2048, NULL, 1,
        &TimeDisplayHandle);  // Пока что используется вместо вывода на матрицу
}

void loop() {}