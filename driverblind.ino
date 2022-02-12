// https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <PCF8574.h>
#include "settings.h"
#include "utils.h"
#include "WindowBlind.h"
#include "DRV8801.h"

void handle_root();
void handle_request();

WebServer server(80);
PCF8574 pcf8574(0x20);
WindowBlind blinds[NUM_OF_BLINDS];
DRV8801 motors[NUM_OF_BLINDS];

void setup()
{
    Serial.begin(9600);
    pcf8574.begin(SDA_PIN, SCL_PIN);

    // Motors setup
    DRV8801PinSetup motor1PinSetup = {DRV1_BRAKE, DRV1_SLEEP, DRV1_DIR, DRV1_PWM, DRV1_FAULT, DRV1_CS};
    motors[0].begin(motor1PinSetup);

    DRV8801PinSetup motor2PinSetup = {DRV2_BRAKE, DRV2_SLEEP, DRV2_DIR, DRV2_PWM, DRV2_FAULT, DRV2_CS};
    motors[1].begin(motor2PinSetup);

    // Window blind setup
    WindowBlindPinSetup blind1PinSetup = {END1_UPR, END1_LWR, UPR1_SW, LWR1_SW, LMT1_UPR, LMT1_LWR};
    WindowBlindTiming blind1Timing = {BLIND1_FULL_UPR_DURATION, BLIND1_FULL_LWR_DURATION};
    blinds[0].begin(blind1PinSetup, blind1Timing, &pcf8574, &motors[0], DRV1_SPEED);

    WindowBlindPinSetup blind2PinSetup = {END2_UPR, END2_LWR, UPR2_SW, LWR2_SW, LMT2_UPR, LMT2_LWR};
    WindowBlindTiming blind2Timing = {BLIND2_FULL_UPR_DURATION, BLIND2_FULL_LWR_DURATION};
    blinds[1].begin(blind2PinSetup, blind2Timing, &pcf8574, &motors[1], DRV2_SPEED);

    // WiFi connection
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print('.');
        delay(1000);
    }
    Serial.println();
    Serial.println(WiFi.localIP());

    // WebServer setup
    server.on("/", handle_root);
    server.on("/request", handle_request);
    server.begin();
}

void loop()
{
    if ((WiFi.status() != WL_CONNECTED) && isEventTriggered(Event::WIFI_RECONNECT))
    {
        for (int i = 0; i < NUM_OF_BLINDS; i++)
        {
            blinds[i].emergencyStop();
        }

        Serial.print(millis());
        Serial.println("Reconnecting to WiFi...");
        WiFi.disconnect();
        WiFi.reconnect();
        eventTimestamps[Event::WIFI_RECONNECT] = millis();
    }

    for (int i = 0; i < NUM_OF_BLINDS; i++)
    {
        blinds[i].handleButtons();
        blinds[i].handleLimiters();
        blinds[i].handleTimeEvents();
        blinds[i].handleMotorFailure();
    }

    server.handleClient();

    delay(2);
}

const char index_html[] PROGMEM = R"===(
    <!doctype html>
    <head>
        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" rel="stylesheet">
        <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0-beta3/css/all.min.css"/>

        <title>Sterownik rolet</title>
    </head>
    <body>
        <div class="container">

            <div class="row justify-content-md-center pt-3">
                <div class="col-lg-2"></div>
                <div class="col-lg-auto">
                    <div class="card text-center">
                        <div class="card-header">
                            Roleta #1
                        </div>
                        <div class="card-body">
                            <div class="row row-cols-2">
                                <div class="vstack gap-2 col-5 mx-auto">
                                    <button type="button" class="btn btn-primary btn-lg" onclick="sendReq('up', 'full', 0)"><i class="fas fa-chevron-up fs-2"></i></button>
                                    <b>FULL</b>
                                    <button type="button" class="btn btn-primary btn-lg" onclick="sendReq('dowm', 'full', 0)"><i class="fas fa-chevron-down fs-2"></i></button>
                                </div>
                                <div class="vstack gap-2 col-5 mx-auto">
                                    <button type="button" class="btn btn-primary btn-lg" onmousedown="sendReq('up', 'part', 0)" onmouseup="sendReq('-', 'stop', 0)"><i class="fas fa-chevron-up fs-2"></i></button>
                                    <b>PART</b>
                                    <button type="button" class="btn btn-primary btn-lg" onmousedown="sendReq('dowm', 'part', 0)" onmouseup="sendReq('-', 'stop', 0)"><i class="fas fa-chevron-down fs-2"></i></button>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
                <div class="col-lg-2"></div>
            </div>

            <div class="row justify-content-md-center pt-3">
                <div class="col-lg-2"></div>
                <div class="col-lg-auto">
                    <div class="card text-center">
                        <div class="card-header">
                            Roleta #2
                        </div>
                        <div class="card-body">
                            <div class="row row-cols-2">
                                <div class="vstack gap-2 col-5 mx-auto">
                                    <button type="button" class="btn btn-primary btn-lg" onclick="sendReq('up', 'full', 1)"><i class="fas fa-chevron-up fs-2"></i></button>
                                    <b>FULL</b>
                                    <button type="button" class="btn btn-primary btn-lg" onclick="sendReq('dowm', 'full', 1)"><i class="fas fa-chevron-down fs-2"></i></button>
                                </div>
                                <div class="vstack gap-2 col-5 mx-auto">
                                    <button type="button" class="btn btn-primary btn-lg" onmousedown="sendReq('up', 'part', 1)" onmouseup="sendReq('-', 'stop', 1)"><i class="fas fa-chevron-up fs-2"></i></button>
                                    <b>PART</b>
                                    <button type="button" class="btn btn-primary btn-lg" onmousedown="sendReq('dowm', 'part', 1)" onmouseup="sendReq('-', 'stop', 1)"><i class="fas fa-chevron-down fs-2"></i></button>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
                <div class="col-lg-2"></div>
            </div>

        </div>

        <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js"></script>
        <script>
            function sendReq(direction, type, blindId)
            {
                var http = new XMLHttpRequest();
                http.open('POST', "/request", true);
                http.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
                http.send("dir=" + direction + "&type=" + type + "&id=" + blindId);
            }
        </script>
    </body>
    </html>
    )===";

void handle_root()
{
    Serial.println("handle root");
    server.send(200, "text/html", index_html);
}

void handle_request()
{
    if (server.hasArg("dir") && server.hasArg("type") && server.hasArg("id"))
    {
        MoveDirection direction = server.arg("dir") == "up" ? MoveDirection::UP : MoveDirection::DOWN;
        String movementType = server.arg("type");
        int blindId = server.arg("id").toInt();

        if (blindId >= 0 && blindId < NUM_OF_BLINDS)
        {
            blinds[blindId].handleHttpRequest(direction, movementType);
            server.send(200);
            return;
        }
    }

    Serial.println("Bad request!");
    server.send(400);
}