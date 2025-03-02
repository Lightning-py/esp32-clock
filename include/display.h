#pragma once

#include "led_matrix/led.h"
#include "updaters/timeUpdater.h"
#include "updaters/weatherUpdater.h"

extern weather weather_data;  // объект для хранения данных погоды

extern Ds1302::DateTime TIME_DISPLAY_VAR;  // объект для хранения данных времени

extern SemaphoreHandle_t
    TIME_DISPLAY_MUTEX;  // мьютекс для доступа к переменной времени
extern SemaphoreHandle_t WEATHER_DISPLAY_MUTEX;

enum state { TIME, WEATHER, DATE };
enum date_state { DATE_MONTH, YEAR, DAY_OF_WEEK };

auto matrix = LedMatrix(std::move(pixels), 32, 8);

void display(void* desktiptor) {
    state st = TIME;
    date_state date_st = DATE_MONTH;

    while (1) {
        switch (st) {
            case TIME:
                for (int i = 0; i < 10; ++i) {
                    int hour, minute;

                    xSemaphoreTake(TIME_DISPLAY_MUTEX, portMAX_DELAY);

                    hour = TIME_DISPLAY_VAR.hour;
                    minute = TIME_DISPLAY_VAR.minute;

                    xSemaphoreGive(TIME_DISPLAY_MUTEX);

                    matrix.clear();
                    matrix.displayTime(hour, minute, 10, 0, 0, i % 2);
                    matrix.update();

                    vTaskDelay(500 / portTICK_PERIOD_MS);
                }

                st = WEATHER;

                break;
            case WEATHER:
                for (int i = 0; i < 6; ++i) {
                    int weather;

                    xSemaphoreTake(WEATHER_DISPLAY_MUTEX, portMAX_DELAY);

                    weather = std::round(weather_data.temp);

                    xSemaphoreGive(WEATHER_DISPLAY_MUTEX);

                    matrix.clear();
                    matrix.displayWeather(weather, 0, 0, 10, i % 2);
                    matrix.update();

                    vTaskDelay(500 / portTICK_PERIOD_MS);
                }

                st = DATE;

                break;
            case DATE:
                for (int i = 0; i < 6; ++i) {
                    int month, day, year, dow;

                    xSemaphoreTake(TIME_DISPLAY_MUTEX, portMAX_DELAY);

                    month = TIME_DISPLAY_VAR.month;
                    day = TIME_DISPLAY_VAR.day;
                    year = TIME_DISPLAY_VAR.year;

                    dow = TIME_DISPLAY_VAR.dow;

                    xSemaphoreGive(TIME_DISPLAY_MUTEX);

                    matrix.clear();
                    matrix.displayDate(day, month, year, dow, date_st, 0, 0, 10,
                                       i % 2 == 0);
                    matrix.update();

                    vTaskDelay(500 / portTICK_PERIOD_MS);
                }

                if (date_st == DATE_MONTH)
                    date_st = YEAR;
                else if (date_st == YEAR)
                    // date_st = DATE_MONTH;
                    date_st = DAY_OF_WEEK;
                else if (date_st == DAY_OF_WEEK)
                    date_st = DATE_MONTH;

                st = TIME;

                break;
        }
    }
}