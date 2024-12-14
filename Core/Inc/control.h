#ifndef _CONTROL_H
#define _CONTROL_H

#include <stdint.h>
#include "touch.h"
#include "objects.h"

#define MOVE_UP 0x01
#define MOVE_DOWN 0x02
#define MOVE_FORWARD 0x04
#define MOVE_BACK 0x08

#define DASH_M 0x10
#define CROUCH_M 0x20
#define JUMP_M 0x40


#define LOGIC_SKIP 2
#define SQ_LOGIC_SKIP 1.2
extern const float MAX_X_SPEED;
extern const float X_SPEED_DEC;
extern const float X_SPEED_DELTA;

extern const float Y_SPEED_JUMP;
extern const float Y_SPEED_UP;
extern const float Y_SPEED_DEC;
// #define MAX_X_SPEED 10
// #define X_SPEED_DEC 0.3
// #define X_SPEED_DELTA 5

// #define Y_SPEED_JUMP 4.9
// #define Y_SPEED_UP 6.9
// #define Y_SPEED_DEC 0.20

#define CROUCH_FRAC 0.75

void update_movement(const _m_tp_dev *p, const list_t *ol); // increase speed at falling edge
void update_elements(list_t *ol);
void update_element(myobj_t *o, list_t *lasp);

void game_logic(list_t *ol);

void update_speed();

#endif