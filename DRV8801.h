#ifndef __DRV8801__
#define __DRV8801__

#include "utils.h"

typedef struct DRV8801PinSetup
{
    int brakePin;
    int sleepPin;
    int dirPin;
    int pwmPin;
    int faultPin;
    int csPin;
} DRV8801PinSetup;

class DRV8801
{
private:
    DRV8801PinSetup pinSetup;
    static int pwmChannelCounter;
    int pwmChannel;

public:
    DRV8801()
    {
        this->pinSetup = {0};
        this->pwmChannel = 0;
    }

    void begin(DRV8801PinSetup _pinSetup)
    {
        this->pinSetup = _pinSetup;
        this->pwmChannel = DRV8801::pwmChannel++;

        pinMode(this->pinSetup.brakePin, OUTPUT);
        pinMode(this->pinSetup.sleepPin, OUTPUT);
        pinMode(this->pinSetup.dirPin, OUTPUT);
        pinMode(this->pinSetup.faultPin, INPUT_PULLUP);
        // pinMode(this->pinSetup.csPin, INPUT);

        ledcSetup(this->pwmChannel, 5000, 8);
        ledcAttachPin(this->pinSetup.pwmPin, this->pwmChannel);

        this->setSleep(true);
        Serial.println((String)"Motor channel: " + this->pwmChannel);
    }

    void setSleep(bool isSleeping)
    {
        digitalWrite(this->pinSetup.sleepPin, !isSleeping);
        Serial.println((String)"Set sleep " + (!isSleeping));
    }

    void move(MoveDirection direction, int speed, bool brakeEnabled)
    {
        this->setSleep(false);
        digitalWrite(this->pinSetup.dirPin, direction);
        ledcWrite(this->pwmChannel, speed);
        digitalWrite(this->pinSetup.brakePin, brakeEnabled);
        Serial.println((String)"Move - direction:" + (int)direction + " speed:" + speed + " brake:" + brakeEnabled);
    }

    void stop()
    {
        ledcWrite(this->pwmChannel, 0);
        digitalWrite(this->pinSetup.brakePin, 0);
        this->setSleep(true);
        Serial.println("Stop motor");
    }

    bool isFaultPresent()
    {
        return !digitalRead(this->pinSetup.faultPin);
    }
};

int DRV8801::pwmChannelCounter = 0;

#endif