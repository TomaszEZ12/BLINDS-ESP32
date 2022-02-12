#ifndef __UTILS__
#define __UTILS__

enum MoveDirection
{
    UP,
    DOWN
};

enum Event
{
    WIFI_RECONNECT,
    EVENT_LENGTH  
};
unsigned long long eventTimestamps[Event::EVENT_LENGTH] = {0};

bool isEventTriggered(Event event)
{
    switch (event)
    {
    case Event::WIFI_RECONNECT:
        return (millis() - eventTimestamps[Event::WIFI_RECONNECT]) >= WIFI_RECONNECT_INTERVAL;
        break;
    }

    return false;
}

#endif