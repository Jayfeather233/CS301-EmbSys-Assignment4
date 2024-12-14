#ifndef _OBJECTS_H
#define _OBJECTS_H

#include <stdint.h>

typedef struct list
{
    void* data;
    struct list *next;
} list_t;

extern list_t obj_list_head;
extern list_t *ground_list_head, *obstacle_list_head, *character_list_head;

void insert_lt(list_t *p, void *o);
void free_lt(list_t *p);
void remove_lt(list_t *p);

typedef struct img{
    const uint16_t *data;
    const uint16_t *mask;
    uint16_t img_x, img_y, scale;
} img_t;

typedef enum object_type{
    OBSTACLE,
    SCORE_OBJ,
    CROUCH_OBJ,
    DASH_OBJ,
    JUMP_OBJ,

    OTT_LEN,

    END_OBJ,
    SOLID_OBJ,
    FIGURE,
    BUTTON,
    STOP_UI,
    GRASS,
    GROUND
} object_type_t;

typedef struct myobj{
    float x, y;
    img_t *data;
    uint16_t st; // status for img [15:4], animation count [3:0]
    object_type_t type;
} myobj_t;

/**
 * walk: 3frame
 * ->stop: 1frame
 * stop: 1frame
 * stop->walk: 1frame
 * ->jump: 3frame
 * jump: 1frame
 * jump->walk: 2frame
 * ->crouch: 1frame
 * crouch: 3frame
 * crouch->walk: 1frame
 * ->dash: 3frame
 * dash: 2frame
 * dash->walk: => crouch->walk
 */

#define ANIME_FPS 1 // fps/anime_fps = real_fps

#define WALK_START 0
#define WALK_LEN 3

#define TO_STOP_START WALK_LEN
#define TO_STOP_LEN 1
#define STOP_START (TO_STOP_START+TO_STOP_LEN)
#define STOP_LEN 1
#define STOP_TO_WALK_START (STOP_START+STOP_LEN)
#define STOP_TO_WALK_LEN 1

#define TO_JUMP_START (STOP_TO_WALK_START+STOP_TO_WALK_LEN)
#define TO_JUMP_LEN 3
#define JUMP_START (TO_JUMP_START+TO_JUMP_LEN)
#define JUMP_LEN 1
#define JUMP_TO_WALK_START (JUMP_START+JUMP_LEN)
#define JUMP_TO_WALK_LEN 2

#define TO_CROUCH_START (JUMP_TO_WALK_START+JUMP_TO_WALK_LEN)
#define TO_CROUCH_LEN 1
#define CROUCH_START (TO_CROUCH_START+TO_CROUCH_LEN)
#define CROUCH_LEN 3
#define CROUCH_TO_WALK_START (CROUCH_START+CROUCH_LEN)
#define CROUCH_TO_WALK_LEN 1

#define TO_DASH_START (CROUCH_TO_WALK_START+CROUCH_TO_WALK_LEN)
#define TO_DASH_LEN 3
#define DASH_START (TO_DASH_START+TO_DASH_LEN)
#define DASH_LEN 2
#define DASH_TO_WALK_START (DASH_START+DASH_LEN)

#define STOP_UI_LEN 4
#define SCORE_LEN 4

typedef enum status{
    MOVING_FORWARD = 0,
    MOVING_BACKWARD = 1,
    STOP = 2,
    CROUCH = 3,
    DASH = 4,
    JUMP = 5,
    IN_AIR = 6
} status_t;

void get_next_status(myobj_t *p, uint16_t movements, status_t *ch_movements, list_t *ol);
const img_t *get_obj_img(const myobj_t* o);
void update_obj(myobj_t *o, status_t s);
myobj_t* new_myobj(uint16_t x, uint16_t y, object_type_t ot);

#endif