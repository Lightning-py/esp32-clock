#pragma once

#define ssid SSID          // название сети WiFi
#define password PASSWORD  // пароль

#define url URL

#define ntpServer \
    "ntp0.ntp-servers.net"  // адрес ntp сервера для обновления времени
// const char *ntpServer = "pool.ntp.org";

#define gmtOffset_sec 3 * 3600  // GMT оффсет

#define LED_PIN 2  // расположение встроенного светодиода на плате

#define PIN_ENA 26  // дата пины у часов Ds1302
#define PIN_CLK 14  //
#define PIN_DAT 27  //

#define PIN 23
#define PIXELS 256

Adafruit_NeoPixel pixels(PIXELS, PIN, NEO_GRB + NEO_KHZ400);

// Идентификаторы задач
TaskHandle_t TimeUpdaterHandler = NULL;    //
TaskHandle_t BlinkHandler = NULL;          //
TaskHandle_t ConnectorHandler = NULL;      //
TaskHandle_t WeatherUpdateHandler = NULL;  //
TaskHandle_t LedRunHandler = NULL;         //

AsyncWebServer server(80);  // веб сервер
String command = "";        // команда с веб сервера

Ds1302 rtc(PIN_ENA, PIN_CLK, PIN_DAT);  // объект для работы с часами Ds1302

// объект для хранения данных времени
Ds1302::DateTime TIME_DISPLAY_VAR = {
    .year = 0,                   //
    .month = Ds1302::MONTH_JAN,  //
    .day = 0,                    //
    .hour = 0,                   //
    .minute = 0,                 //
    .second = 0,                 //
    .dow = Ds1302::DOW_SUN       //
};  //

// мьютекс для
SemaphoreHandle_t TIME_DISPLAY_MUTEX;  // -- доступа к переменной времени
SemaphoreHandle_t WEATHER_DISPLAY_MUTEX;  // --  к переменной погоды

//  Размер стека задачи по
#define TIME_UPDATE_STACK_SIZE 2048  // -- обновлению времени
#define BLINK_STACK_SIZE 1024        // -- по свечению
#define CONNECTOR_STACK_SIZE 2048  // -- по соединению с интернетом
#define WEATHER_UPDATE_STACK_SIZE 4096  // -- по обновлению погоды
#define LED_RUN_STACK_SIZE 4096  // -- по выводу на светодиодную матрицу

const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width,
    initial-scale=1.0"> <title>Кнопки для запросов</title> <script>
        function sendRequest(command) {
            fetch(`/c/?command=${command}`)
                .then(response => {
                    if (!response.ok) {
                        throw new Error('Сеть не в порядке');
                    }
                    return response.json();
                })
                .then(data => {
                    console.log(data);
                    alert(`Ответ: ${JSON.stringify(data)}`);
                })
                .catch(error => {
                    console.error('Ошибка:', error);
                    alert('Произошла ошибка: ' + error.message);
                });
        }
    </script>
</head>
<body>
    <h1>Отправка запросов</h1>
    <button onclick="sendRequest('stripe')">Отправить Stripe</button>
    <button onclick="sendRequest('remaining_stripe')">Отправить RemainingStripe</button>
    <button onclick="sendRequest('snake')">Отправить Snake</button>
</body>
</html>
)rawliteral";
