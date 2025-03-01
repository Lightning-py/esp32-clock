#pragma once

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#include <sstream>

#include "info.h"
#include "pictures.h"

class LedMatrix {
   private:
    Adafruit_NeoPixel pixel_object;
    int LED_X;
    int LED_Y;

   public:
    explicit LedMatrix(Adafruit_NeoPixel&& pixel_object, int LED_X, int LED_Y)
        : pixel_object(pixel_object), LED_X(LED_X), LED_Y(LED_Y) {}

    void setColor(int x, int y, int r, int g, int b) {
        if (x % 2 == 0) {
            // столбец четный, значит идет вниз
            pixel_object.setPixelColor(x * LED_Y + y,
                                       pixel_object.Color(r, g, b));
        } else {
            // столбец нечетный, значит идет вверх
            pixel_object.setPixelColor(x * LED_Y + (LED_Y - y - 1),
                                       pixel_object.Color(r, g, b));
        }
    }

    void update() { pixel_object.show(); }
    void clear() { pixel_object.clear(); }

    void displayPicture(picture pic, int x_start, int y_start, int r, int g,
                        int b) {
        if (pic.x + x_start > LED_X || pic.y + y_start > LED_Y)
            throw std::runtime_error("Matrix out of bounds");

        for (int i = 0; i < pic.y; ++i) {
            for (int j = 0; j < pic.x; ++j) {
                if (pic.picture[i][j]) {
                    setColor(x_start + j, y_start + i, r, g, b);
                }
            }
        }
    }

    void displayTime(int hour, int minute, int r, int g, int b,
                     bool display_colon) {
        std::ostringstream oss;
        oss << hour / 10 << " " << hour % 10 << " "
            << (display_colon ? ":" : " ") << " " << minute / 10 << " "
            << minute % 10;
        std::string result = oss.str();

        putMiddleText(result, r, g, b, 0);
    }

    void displayWeather(int number, int r, int g, int b, bool display_celsium) {
        std::ostringstream oss;
        if (number < 0) {
            oss << "- ";
        }

        oss << std::abs(number);

        std::string result = oss.str();

        int start_pos = putMiddleText(result, r, g, b, (celsium.x + 1) / 2) + 1;

        if (display_celsium)
            displayPicture(celsium, start_pos, 0, r, g, b);
        else
            displayPicture(celsium_off, start_pos, 0, r, g, b);
    }

    void displayDate(int day, int month, int year, int dow, int regime, int r,
                     int g, int b, bool display_dot) {
        if (regime == 0) {
            year += 2000;

            std::ostringstream oss;
            oss << (year / 1000) << " " << (year / 100) % 10 << " "
                << (year / 10) % 10 << " " << year % 10;

            std::string result = oss.str();

            putMiddleText(result, r, g, b, 0);
        } else if (regime == 1) {
            std::ostringstream oss;
            oss << day / 10 << " " << day % 10 << " "
                << (display_dot ? "." : " ") << " " << month / 10 << " "
                << month % 10;

            std::string result = oss.str();

            putMiddleText(result, r, g, b, 0);
        } else if (regime == 2) {
            std::ostringstream oss;

            switch (dow) {
                case 1:
                    oss << "M O N";
                    break;
                case 2:
                    oss << "T U E";
                    break;
                case 3:
                    oss << "W E D";
                    break;
                case 4:
                    oss << "T H U";
                    break;
                case 5:
                    oss << "F R I";
                    break;
                case 6:
                    oss << "S A T";
                    break;
                case 7:
                    oss << "S U N";
                    break;
            }

            std::string result = oss.str();

            putMiddleText(result, r, g, b, 0);
        }
    }

    int putText(std::string& str, int x, int r, int g, int b) {
        for (auto& chr : str) {
            picture p = getSymbol(chr);

            displayPicture(p, x, 0, r, g, b);
            x += p.x;
        }

        return x;
    }

    int putMiddleText(std::string& str, int r, int g, int b, int offset) {
        return putText(str, (LED_X - getSize(str)) / 2 - offset, r, g, b);
    }
};