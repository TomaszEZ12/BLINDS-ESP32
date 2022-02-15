#include "utils.h"
#include "DRV8801.h"
#include <PCF8574.h>

typedef struct WindowBlindPinSetup
{
   
    // Buttons
    int UPERSwitchPin;
    int lowerSwitchPin;
    // Limit switches
    int limitTOPPin;
    int limitDOWNPin;
} WindowBlindPinSetup;

typedef struct WindowBlindTiming
{
    int uprFullDuration;
    int lwrFullDuration;
} WindowBlindTiming;

class WindowBlind
{
private:
    enum State
    {
        IDLE,
        MOVING_DOWN,
        MOVING_UP,
        MOVING_PARTLY
    };

    WindowBlindPinSetup pinSetup;
    WindowBlindTiming timing;
    PCF8574* pcf8574;
    DRV8801* motor;
    State state;
    unsigned long long motorStartTime;
    int motorSpeed;

    void move(MoveDirection direction)
    {
        this->motor->move(direction, this->motorSpeed, false);
        this->motorStartTime = millis();
        Serial.println("Move motor");
    }

public:
    WindowBlind()
    {
        this->pinSetup = {0};
        this->timing = {0};
        this->pcf8574 = nullptr;
        this->motor = nullptr;
        this->state = IDLE;
        this->motorStartTime = 0;
        this->motorSpeed = 0;
    }

    void begin(WindowBlindPinSetup _pinSetup, WindowBlindTiming _timing, PCF8574* _pcf8574, DRV8801* _motor, int _motorSpeed)
    {
        this->pinSetup = _pinSetup;
        this->timing = _timing;
        this->pcf8574 = _pcf8574;
        this->motor = _motor;
        this->state = IDLE;
        this->motorSpeed = _motorSpeed;

        // Buttons
        pinMode(this->pinSetup.uprSwPin, INPUT);
        pinMode(this->pinSetup.lwrSwPin, INPUT);

        // Limit switches
        pinMode(this->pinSetup.lmtUprPin, INPUT);
        pinMode(this->pinSetup.lmtLwrPin, INPUT);
    }

    void handleButtons()
    {
        if (this->state != MOVING_UP && digitalRead(this->pinSetup.uprSwPin) == LOW)
        {
            this->state = MOVING_UP;
            motorStartTime = millis();
            this->move(MoveDirection::UP);
            Serial.println("Button UP pressed");
        }
        else if (this->state != MOVING_DOWN && digitalRead(this->pinSetup.lwrSwPin) == LOW)
        {
            this->state = MOVING_DOWN;
            motorStartTime = millis();
            this->move(MoveDirection::DOWN);
            Serial.println("Button DOWN pressed");
        }
    }

    void handleLimiters()
    {
        if ((this->state == MOVING_UP && digitalRead(this->pinSetup.lmtUprPin) == LOW) ||
            (this->state == MOVING_DOWN && digitalRead(this->pinSetup.lmtLwrPin) == LOW))
        {
            this->motor->stop();
            this->state = IDLE;
            this->motorStartTime = 0;
            Serial.println("Stop motor (limiter)");
        }

        this->pcf8574->write(this->pinSetup.endUprPin, !digitalRead(this->pinSetup.lmtUprPin));
        this->pcf8574->write(this->pinSetup.endLwrPin, !digitalRead(this->pinSetup.lmtLwrPin));
    }

    void handleTimeEvents()
    {
        if (this->state != IDLE && this->state != MOVING_PARTLY)
        {
            int elapsedTime = millis() - this->motorStartTime;

            if ((this->state == MOVING_UP && elapsedTime >= this->timing.uprFullDuration) ||
                (this->state == MOVING_DOWN && elapsedTime >= this->timing.lwrFullDuration))
            {
                this->motor->stop();
                this->state = IDLE;
                this->motorStartTime = 0;
                Serial.println("Stop motor (time event)");
            }
        }
    }

    void emergencyStop()
    {
        this->motor->stop();
        this->state = IDLE;
        this->motorStartTime = 0;
        Serial.println("Emergency motor STOP!");
    }

    void handleMotorFailure()
    {
        if (this->motor->isFaultPresent())
        {
            //Serial.println("Motor driver failure detected!");
            //this->emergencyStop();
        }
    }

    void handleHttpRequest(MoveDirection direction, String movementType)
    {
        if (movementType == "stop")
        {
            Serial.println("Http request STOP");
            this->motor->stop();
            this->state = IDLE;
            this->motorStartTime = 0;
        }
        else if (movementType == "full")
        {
            Serial.println("Http request FULL");
            this->state = direction == MoveDirection::UP ? MOVING_UP : MOVING_DOWN;
            motorStartTime = millis();
            this->move(direction);
        }
        else if (movementType == "part")
        {
            Serial.println("Http request PART");
            this->state = MOVING_PARTLY;
            this->move(direction);
        }
    }
};
