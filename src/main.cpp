
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <Ds1302.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <WiFi.h>

#include <string>

#include "blink.h"
#include "display.h"
#include "handler.h"
#include "info.h"  // файл с конфигурационными данными, в том числе паролем WiFi, url с локацией и тому подобной информации
#include "time.h"
#include "updaters/connector.h"
#include "updaters/timeUpdater.h"
#include "updaters/weatherUpdater.h"

weather weather_data = {0.0, -1, false};  // объект для хранения данных погоды

void setup() {
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

    xTaskCreate(Blink,             //
                "Blink",           //
                BLINK_STACK_SIZE,  //
                NULL,              //
                1,                 //
                &BlinkHandler      //
    );

    xTaskCreate(connect,               //
                "Wifi connection",     //
                CONNECTOR_STACK_SIZE,  //
                NULL,                  //
                1,                     //
                &ConnectorHandler      //
    );

    xTaskCreate(Continious_time_updater,  //
                "Time update",            //
                TIME_UPDATE_STACK_SIZE,   //
                NULL,                     //
                1,                        //
                &TimeUpdaterHandler       //
    );

    xTaskCreate(weather_update,             //
                "Weather update",           //
                WEATHER_UPDATE_STACK_SIZE,  //
                NULL,                       //
                1,                          //
                &WeatherUpdateHandler       //
    );

    xTaskCreate(display,             //
                "Led run",           //
                LED_RUN_STACK_SIZE,  //
                NULL,                //
                1,                   //
                &LedRunHandler       //
    );

    // --------------------------------------------------------------------------------------------------------------------------------

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", htmlPage);
    });

    server.on("/c",      //
              HTTP_GET,  //
              [](AsyncWebServerRequest *request) {
                  if (request->hasParam("command")) {
                      command =
                          request->getParam("command")
                              ->value();  // Получение команды из параметра

                      auto result =
                          std::to_string(handler(std::string(command.c_str()) +
                                                 std::string("\n")))
                              .c_str();

                      request->send(200, "text/plain", result);
                  } else {
                      request->send(400, "text/plain",
                                    "Bad Request: command not specified");
                  }
              }  //
    );

    server.begin();
}

void loop() {}