# BLINDS-ESP32 ARDUINO IDE / WEBSERVER
Automatic home roller shutter software / Oprogramowanie automatycznej rolety Arduino IDE

# Budowa rolety:
Mechanizm rolety został zbudowany na bazie ESP32 WROOM wraz z modułem sterownika silników DRV8801.

Łączność z roletą odbywa się przez sieć Wi-Fi. Dane dostępowe do routera przechowywane są wraz z innymi informacjami w pamięci flash. W przypadku braku informacji o sieci, urządzenie nawiązuje próbę połączenia co 10sekund.

Roleta automatycznie łączy się z zaprogramowaną siecią Wi-Fi , wyświettlając IP modułu ESP32.


Urządzenie posiada opcję zmiany kierunku obrotów silnika oraz możliwość kalibracji rolet. Program nie wyklucza stosowania ograniczników krańcowych (kontaktronów).

Roleta może zostać zatrzymana poprez fukcje czasowe. Dokładnie po 15sekundach, wartość czasową można dostosować do długości rolety/szybkości silnika.

# Sterowanie

Manualnie poprez przyciski oraz WebServer.

Sterowanie urządzeniem odbywa się poprzez wykorzystanie metod dostępnych w protokole HTTP. Sterować można z przeglądarki na komputerze lub smartfonie.

Serwer HTTP, będzie w stanie obsługiwać 4 rolety - potrzebne będą 4 drivery silnika.

Aktualnie KOD dla 2 driverów drv8801.

Użyte biblioteki:
https://github.com/RobTillaart/PCF8574

https://github.com/espressif/arduino-esp32/tree/master/libraries/WebServer

https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi
