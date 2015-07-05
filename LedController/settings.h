#ifndef SETTINGS_H
#define SETTINGS_H

typedef uint8_t palind_t;
typedef uint8_t ledind_t;
typedef int32_t speed_t;
typedef int32_t accel_t;
typedef int32_t colind_t;
typedef uint8_t prob_t;
typedef uint16_t event_t;
typedef int32_t pos_t;
typedef uint8_t col_t;

#define PROB_MAX 256
#define MAX_ACCELERATION 256
#define FPS 200
#define POS_PRECISION 1000
#define FRAME_DELAY (1000 / FPS)
#define MIN_SPEED 100
#define MAX_SPEED 1e12

#define IR_CMD_UP 0x9A
#define IR_CMD_LEFT 0xA8
#define IR_CMD_DOWN 0x07
#define IR_CMD_RIGHT 0xEE
#define IR_CMD_ENTER 0x24
#define IR_CMD_EXIT 0xCD
#define IR_CMD_0 0x8B
#define IR_CMD_1 0xF7
#define IR_CMD_2 0x7B
#define IR_CMD_3 0xB1
#define IR_CMD_4 0xD4
#define IR_CMD_5 0x58
#define IR_CMD_6 0x92
#define IR_CMD_7 0x6A
#define IR_CMD_8 0xE6
#define IR_CMD_9 0x2C

#define NUM_COLORS_PER_PALETTE 5  // # of base colors per palette
#define GRADIENT_SIZE 256         // # of shades in gradient between each base color
#define PALETTE_SIZE_BLOCKS NUM_COLORS_PER_PALETTE     // # of blocks of GRADIENT_SIZE
#define PALETTE_SIZE (PALETTE_SIZE_BLOCKS*GRADIENT_SIZE) // # of colors per palette (must be multiple of 256)
#define PALETTE_CHANGE_MS 10000
#define NUM_PALETTES 201
#define MIN_SATURATION 0

#define NUM_LEDS 60
#define DATA_PIN 10

#define p(...) Serial.print(__VA_ARGS__)

#endif
