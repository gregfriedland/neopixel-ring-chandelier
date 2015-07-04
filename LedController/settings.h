#ifndef SETTINGS_H
#define SETTINGS_H

#define PROB_MAX 256
#define MAX_SPEED 1500
#define MAX_ACCELERATION 256
#define FPS 200
#define POS_PRECISION 200
#define FRAME_DELAY (1000 / FPS)

#define IR_CMD_UP 0x9A
#define IR_CMD_LEFT 0x3B
#define IR_CMD_DOWN 0x73
#define IR_CMD_RIGHT 0x05
#define IR_CMD_ENTER 0x24
#define IR_CMD_EXIT 0xCD

#define PALETTE_SIZE 1024
#define PALETTE_CHANGE_MS 10000
#define NUM_PALETTES 200

#define NUM_LEDS 60
#define DATA_PIN 10

#endif
