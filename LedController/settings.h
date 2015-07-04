#ifndef SETTINGS_H
#define SETTINGS_H

typedef uint8_t ledind_t;
typedef int16_t speed_t;
typedef int8_t accel_t;
typedef uint16_t colind_t;
typedef uint8_t prob_t;
typedef uint16_t event_t;
typedef uint32_t pos_t;

#define PROB_MAX 256
#define MAX_SPEED 1500
#define MAX_ACCELERATION 256
#define FPS 200
#define POS_PRECISION 1000
#define FRAME_DELAY (1000 / FPS)

#define IR_CMD_UP 0x9A
#define IR_CMD_LEFT 0x3B
#define IR_CMD_DOWN 0x73
#define IR_CMD_RIGHT 0x05
#define IR_CMD_ENTER 0x24
#define IR_CMD_EXIT 0xCD

#define NUM_COLORS_PER_PALETTE 5
#define PALETTE_SIZE (NUM_COLORS_PER_PALETTE*256)
#define PALETTE_CHANGE_MS 10000
#define NUM_PALETTES 200

#define NUM_LEDS 60
#define DATA_PIN 10

#define p(...) //Serial.print(__VA_ARGS__)

#endif
