#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "password"

#define WIFI_RECONNECT_INTERVAL 10000
#define NUM_OF_BLINDS 2

// Buttons
#define LWR1_SW 2
#define LWR2_SW 3
#define UPR1_SW 7
#define UPR2_SW 6

// Limit switches
#define LMT1_UPR 8
#define LMT2_UPR 5
#define LMT1_LWR 8
#define LMT2_LWR 4
// PCF8574
#define SDA_PIN 22
#define SCL_PIN 21

// Motor 1
#define DRV1_BRAKE 11
#define DRV1_SLEEP 12
#define DRV1_DIR 13
#define DRV1_PWM 14
#define DRV1_FAULT 15
#define DRV1_CS 16
#define DRV1_SPEED 255

// Motor 2
#define DRV2_BRAKE 17
#define DRV2_SLEEP 18
#define DRV2_DIR 19
#define DRV2_PWM 20
#define DRV2_FAULT 25
#define DRV2_CS 26
#define DRV2_SPEED 255

// Full blinds close/open duration (ms)
#define BLIND1_FULL_UPR_DURATION 15000
#define BLIND1_FULL_LWR_DURATION BLIND1_FULL_UPR_DURATION
#define BLIND2_FULL_UPR_DURATION 15000
#define BLIND2_FULL_LWR_DURATION BLIND2_FULL_UPR_DURATION
