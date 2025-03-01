#pragma once

#include <Arduino.h>
#include <Ds1302.h>
#include <WiFi.h>

#include "info.h"
#include "time.h"

extern Ds1302 rtc;
extern Ds1302::DateTime TIME_DISPLAY_VAR;
extern SemaphoreHandle_t TIME_DISPLAY_MUTEX;

void updateTime(WiFiEvent_t event, WiFiEventInfo_t info) {
    struct tm tm1;
    getLocalTime(&tm1);

#ifdef DEBUG

    Serial.printf("\nTime was %d.%d.%d %d:%d\n", tm1.tm_year, tm1.tm_mon,
                  tm1.tm_mday, tm1.tm_hour, tm1.tm_min);
#endif  // DEBUG

    configTime(gmtOffset_sec, 0, ntpServer);

    struct tm tm2;
    getLocalTime(&tm2);

#ifdef DEBUG
    Serial.printf("\nTime became %d.%d.%d %d:%d\n", tm2.tm_year, tm2.tm_mon,
                  tm2.tm_mday, tm2.tm_hour, tm2.tm_min);
#endif  // DEBUG

    if (tm1.tm_year != tm2.tm_year || tm1.tm_mon != tm2.tm_mon ||
        tm1.tm_mday != tm2.tm_mday || tm1.tm_hour != tm2.tm_hour ||
        tm1.tm_sec != tm2.tm_sec) {
#ifdef DEBUG
        Serial.println("\nUpdating rtc time\n");
#endif  // DEBUG

        Ds1302::DateTime dt = {.year = (uint8_t)tm2.tm_year,
                               .month = Ds1302::MONTH_FEB,
                               .day = (uint8_t)tm2.tm_mday,
                               .hour = (uint8_t)tm2.tm_hour,
                               .minute = (uint8_t)tm2.tm_min,
                               .second = (uint8_t)tm2.tm_sec,
                               .dow = Ds1302::DOW_SUN};

        switch (tm2.tm_mon) {
            case 0:
                dt.month = Ds1302::MONTH_JAN;
                break;
            case 1:
                dt.month = Ds1302::MONTH_FEB;
                break;
            case 2:
                dt.month = Ds1302::MONTH_MAR;
                break;
            case 3:
                dt.month = Ds1302::MONTH_APR;
                break;
            case 4:
                dt.month = Ds1302::MONTH_MAY;
                break;
            case 5:
                dt.month = Ds1302::MONTH_JUN;
                break;
            case 6:
                dt.month = Ds1302::MONTH_JUL;
                break;
            case 7:
                dt.month = Ds1302::MONTH_AUG;
                break;
            case 8:
                dt.month = Ds1302::MONTH_SET;
                break;
            case 9:
                dt.month = Ds1302::MONTH_OCT;
                break;
            case 10:
                dt.month = Ds1302::MONTH_NOV;
                break;
            case 11:
                dt.month = Ds1302::MONTH_DEC;
                break;
            default:
                break;
        }

        switch (tm2.tm_wday) {
            case 1:
                dt.dow = Ds1302::DOW_MON;
                break;
            case 2:
                dt.dow = Ds1302::DOW_TUE;
                break;
            case 3:
                dt.dow = Ds1302::DOW_WED;
                break;
            case 4:
                dt.dow = Ds1302::DOW_THU;
                break;
            case 5:
                dt.dow = Ds1302::DOW_FRI;
                break;
            case 6:
                dt.dow = Ds1302::DOW_SAT;
                break;
            case 0:
                dt.dow = Ds1302::DOW_SUN;
                break;
            default:
                break;
        }
        rtc.setDateTime(&dt);
    }
}

void Continious_time_updater(void *pvParameters) {
    Ds1302::DateTime now;

    while (1) {
        rtc.getDateTime(&now);

        xSemaphoreTake(TIME_DISPLAY_MUTEX, portMAX_DELAY);

        TIME_DISPLAY_VAR = now;

        xSemaphoreGive(TIME_DISPLAY_MUTEX);

        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Задержка 1 секунда
    }
}