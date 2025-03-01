
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <Ds1302.h>
// #include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <WiFi.h>

#include <string>

#include "display.h"
// #include "handler.h"
#include "info.h"  // файл с конфигурационными данными, в том числе паролем WiFi, url с локацией и тому подобной информации
#include "time.h"
#include "updaters/blink.h"
#include "updaters/connector.h"
#include "updaters/timeUpdater.h"
#include "updaters/weatherUpdater.h"

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
TaskHandle_t LedRun = NULL;

#define TIME_UPDATE_STACK_SIZE \
    2048  // Размер стека задачи по обновлению времени
#define BLINK_STACK_SIZE 1024  // -- по свечению
#define CONNECTOR_STACK_SIZE 2048  // -- по соединению с интернетом
#define WEATHER_UPDATE_STACK_SIZE 4096  // -- по обновлению погоды

// AsyncWebServer server(80);
// String command = "";

void setup() {
    pixels.begin();

    Serial.begin(9600);

    rtc.init();  // инициализация RTC

    pinMode(LED_PIN, OUTPUT);  // бинд порта светодиода на вывод

    TIME_DISPLAY_MUTEX =
        xSemaphoreCreateMutex();  // создание мьютекса для обновления времени
    WEATHER_DISPLAY_MUTEX = xSemaphoreCreateMutex();  // -- погоды

    WiFi.onEvent(
        updateTime,
        ARDUINO_EVENT_WIFI_STA_CONNECTED);  // обновление времени по NTP
    // после
    // подключения к WiFi

    xTaskCreate(Blink, "Blink", BLINK_STACK_SIZE, NULL, 1, &BlinkHandle);
    xTaskCreate(connect, "Wifi", CONNECTOR_STACK_SIZE, NULL, 1,
                &ConnectorHandle);
    xTaskCreate(Continious_time_updater, "Time update", TIME_UPDATE_STACK_SIZE,
                NULL, 1, &Task1Handle);
    xTaskCreate(weather_update, "Weather update", WEATHER_UPDATE_STACK_SIZE,
                NULL, 1, &WeatherUpdateHandle);

    // server.on("/command", HTTP_GET, [](AsyncWebServerRequest *request) {
    //     if (request->hasParam("value")) {
    //         command = request->getParam("value")
    //                       ->value();  // Получение команды из параметра

    //         auto result = std::to_string(handler(command.c_str())).c_str();

    //         request->send(200, "text/plain", result);
    //     } else {
    //         request->send(400, "text/plain",
    //                       "Bad Request: command not specified");
    //     }
    // });

    // server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request) {
    //     xSemaphoreTake(WEATHER_DISPLAY_MUTEX, portMAX_DELAY);

    //     double temp = weather_data.temp;

    //     xSemaphoreGive(WEATHER_DISPLAY_MUTEX);

    //     request->send(200, "text/plain", std::to_string(temp).c_str());
    // });

    // server.begin();

    // --------------------------------------------------------------------------------------------------------------------------------

    xTaskCreate(display, "Led run", 4096, NULL, 1, &LedRun);
}

void loop() {}