#include "objects.h"
#include "control.h"
#include "main.h"
#include "lcd.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdlib.h>

extern float speed_x, speed_y;
extern float total_x, end_position;

void insert_lt(list_t *p, void *o){
    if(p==NULL){
        return;
    }
    list_t *pp = p->next;
    p->next = (list_t*)malloc(sizeof(list_t));
    p->next->data = o;
    p->next->next = pp;
}

void free_obj(myobj_t *p){
    if(p == NULL) return;
    free(p->data);
    free(p);
}
void free_lt(list_t *p){
    if (p == NULL) return;
    free_obj(p->data);
    free_lt(p->next);
    free(p);
}
void remove_lt(list_t *p){
    if(p == NULL) return;
    if(p->next == NULL) return;
    list_t *pp = p->next->next;
    p->next->next = NULL;
    free_lt(p->next);
    p->next = pp;
}

const img_t *get_obj_img(const myobj_t* o){
    return &(o->data[o->st>>4]);
}
void update_obj(myobj_t *o, status_t s){
    uint16_t ost = (o->st>>4);
    if (o->type == FIGURE){
        switch (s)
        {
        case MOVING_FORWARD:
        case MOVING_BACKWARD: //TODO revese-if to get better performance
            if (ost < TO_STOP_START) {
                ++ o->st;
                if ((o->st & 15) == ANIME_FPS) {
                    o->st = (ost+1) << 4;
                }
                if (o->st == (TO_STOP_START<<4)) {
                    o->st = 0;
                }
            } else if (TO_STOP_START <= ost && ost < STOP_TO_WALK_START){ // stop
                o->st = STOP_TO_WALK_START << 4;
            } else if (STOP_TO_WALK_START <= ost && ost < TO_JUMP_START){ // from stop to walk
                ++o->st;
                if ((o->st & 15) == ANIME_FPS){
                    o->st = (ost+1) << 4;
                }
                if (o->st == (TO_JUMP_START << 4)) {
                    o->st = 0;
                }
            } else if (TO_JUMP_START <= ost && ost < JUMP_TO_WALK_START){
                o->st = JUMP_TO_WALK_START << 4;
            } else if (JUMP_TO_WALK_START <= ost && ost < TO_CROUCH_START){
                ++o->st;
                if ((o->st & 15) == ANIME_FPS){
                    o->st = (ost+1) << 4;
                }
                if (o->st == (TO_CROUCH_START << 4)) {
                    o->st = 0;
                }
            } else if (TO_CROUCH_START <= ost && ost < CROUCH_TO_WALK_START){
                o->st = CROUCH_TO_WALK_START << 4;
            } else if (CROUCH_TO_WALK_START <= ost && ost < TO_DASH_START){
                ++o->st;
                if ((o->st & 15) == ANIME_FPS){
                    o->st = (ost+1) << 4;
                }
                if (o->st == (TO_DASH_START << 4)) {
                    o->st = 0;
                }
            } else if (TO_DASH_START <= ost && ost < DASH_TO_WALK_START){
                o->st = CROUCH_TO_WALK_START << 4;
            } else {
            }
            break;
        
        case STOP:
            if (TO_STOP_START <= ost && ost < STOP_TO_WALK_START){
                ++o->st;
                if ((o->st & 15) == ANIME_FPS){
                    o->st = (ost+1)<<4;
                }
                if (o->st == (STOP_TO_WALK_START<<4)){
                    o->st = STOP_START<<4;
                }
            } else {
                o->st = TO_STOP_START<<4;
            }
            break;
        
        case CROUCH:
            if (TO_CROUCH_START <= ost && ost < CROUCH_TO_WALK_START){
                ++o->st;
                if ((o->st & 15) == ANIME_FPS){
                    o->st = (ost+1)<<4;
                }
                if (o->st == (CROUCH_TO_WALK_START<<4)){
                    o->st = CROUCH_START<<4;
                }
            } else {
                o->st = TO_CROUCH_START<<4;
            }
            break;

        case DASH:
            if (TO_DASH_START <= ost && ost < DASH_TO_WALK_START){
                ++o->st;
                if ((o->st & 15) == ANIME_FPS){
                    o->st = (ost+1)<<4;
                }
                if (o->st == (DASH_TO_WALK_START<<4)){
                    o->st = DASH_START<<4;
                }
            } else {
                o->st = TO_DASH_START<<4;
            }
            break;
        case JUMP:
            if (TO_JUMP_START <= ost && ost < JUMP_TO_WALK_START){
                ++o->st;
                if ((o->st & 15) == ANIME_FPS){
                    o->st = (ost+1)<<4;
                }
                if (o->st == (JUMP_TO_WALK_START<<4)){
                    o->st = JUMP_START<<4;
                }
            } else {
                o->st = TO_JUMP_START<<4;
            }
            break;
        
        case IN_AIR:
            if (JUMP_START <= ost && ost < JUMP_TO_WALK_START){
                ++o->st;
                if ((o->st & 15) == ANIME_FPS){
                    o->st = (ost+1)<<4;
                }
                if (o->st == (JUMP_TO_WALK_START<<4)){
                    o->st = JUMP_START<<4;
                }
            } else {
                o->st = JUMP_START<<4;
            }
            break;
        
        
        default:
            break;
        }
    } else if (o->type == STOP_UI){
        ++o->st;
        if ((o->st & 15) == ANIME_FPS){
            o->st = (ost+1)<<4;
        }
        if (o->st == STOP_UI_LEN<<4){
            o->st = (STOP_UI_LEN-1)<<4;
        }
    } else if (o->type == SCORE_OBJ) {
        ++o->st;
        if ((o->st & 15) == ANIME_FPS){
            o->st = (ost+1)<<4;
        }
        if (o->st == SCORE_LEN<<4){
            o->st = 0;
        }
    } else {
        o->st = 0;
    }
}

static const uint16_t *ref_grass[5] = {grass1, grass2, grass3, grass4, grass5};
static const uint16_t *ref_grass_trans[5] = {grass1_trans, grass2_trans, grass3_trans, grass4_trans, grass5_trans};

static const uint16_t *ref_ground[2] = {ground1, ground2};
static const uint16_t *ref_ground_trans[2] = {ground1_trans, ground2_trans};

myobj_t* new_myobj(uint16_t x, uint16_t y, object_type_t ot){
    // TODO: scence_id
    myobj_t *p = (myobj_t*)malloc(sizeof(myobj_t));
    p->type = ot;
    p->x = x;
    p->y = y;
    p->st = 0;
    if (ot == OBSTACLE){
        p->data = (img_t*)malloc(sizeof(img_t));
        p->data->data = obstacle;
        p->data->img_x = p->data->img_y = 72;
        p->data->mask = obstacle_trans;
        p->data->scale = 1;
    } else if (ot == SCORE_OBJ) {
        p->data = (img_t*)malloc(sizeof(img_t) * SCORE_LEN);
        for(uint16_t i=0;i<SCORE_LEN;++i){
            p->data[i].data = score_obj[i];
            p->data[i].img_x = p->data[i].img_y = 32;
            p->data[i].mask = score_obj_trans[i];
            p->data[i].scale = 1;
        }
        p->x += 10;
    } else if (ot == CROUCH_OBJ){
        p->data = (img_t*)malloc(sizeof(img_t));
        p->data->data = obstacle_crouch;
        p->data->img_x = p->data->img_y = 72;
        p->data->mask = obstacle_crouch_trans;
        p->data->scale = 1;
    } else if (ot == DASH_OBJ){
        p->data = (img_t*)malloc(sizeof(img_t));
        p->data->data = obstacle_dash;
        p->data->img_x = p->data->img_y = 72;
        p->data->mask = obstacle_dash_trans;
        p->data->scale = 1;
    } else if (ot == JUMP_OBJ){
        p->data = (img_t*)malloc(sizeof(img_t));
        p->data->data = obstacle_jump;
        p->data->img_x = p->data->img_y = 72;
        p->data->mask = obstacle_jump_trans;
        p->data->scale = 1;
    } else if (ot == END_OBJ){
        p->data = (img_t*)malloc(sizeof(img_t));
        p->data->data = end_obj;
        p->data->img_x = p->data->img_y = 72;
        p->data->mask = end_obj_trans;
        p->data->scale = 1;
    } else if (ot == SOLID_OBJ){
        p->data = (img_t*)malloc(sizeof(img_t));
        p->data->data = solid_obj;
        p->data->img_x = p->data->img_y = 72;
        p->data->mask = solid_obj_trans;
        p->data->scale = 1;
    } else if (ot == FIGURE) {
        p->data = (img_t*)malloc(sizeof(img_t) * DASH_TO_WALK_START);
        for(uint16_t i=0;i<DASH_TO_WALK_START;++i){
            p->data[i].data = slugcat_run[i];
            p->data[i].img_x = p->data[i].img_y = 32;
            p->data[i].mask = slugcat_run_trans[i];
            p->data[i].scale = 1;
        }
    } else if (ot == BUTTON) {
        p->data = (img_t*)malloc(sizeof(img_t));
        p->data->scale = 1;
    } else if (ot == STOP_UI) {
        p->data = (img_t*)malloc(sizeof(img_t) * STOP_UI_LEN);
        for(uint16_t i=0;i<STOP_UI_LEN;++i){
            p->data[i].data = stop[i];
            p->data[i].img_x = p->data[i].img_y = 16;
            p->data[i].mask = stop_trans[i];
            p->data[i].scale = 1;
        }
    } else if (ot == GRASS) {
        p->data = (img_t*)malloc(sizeof(img_t));
        uint16_t id = rand()%5;
        p->data->data = ref_grass[id];
        p->data->img_x = p->data->img_y = 72;
        p->data->mask = ref_grass_trans[id];
        p->data->scale = 1;
    } else if (ot == GROUND) {
        p->data = (img_t*)malloc(sizeof(img_t));
        uint16_t id = rand()%2;
        p->data->data = ref_ground[id];
        p->data->img_x = 320;
        p->data->img_y = 20;
        p->data->mask = ref_ground_trans[id];
        p->data->scale = 1;
    }
    return p;
}

uint16_t movements_frame_cnt = 0; // count for dash, crouch

void get_next_status(myobj_t *p, uint16_t movements, status_t *ch_movements, list_t *ol){
    uint16_t linep = (p->y <= 56 ? 0 : (p->y <= 136 ? 1 : 2));
    myobj_t *pobj;
    uint8_t is_in = 0;
    list_t *las = ol;
    list_t *oll = NULL;
    list_t *ol1 = NULL, *ol2 = NULL, *ol3 = NULL;
    while(ol != NULL){
        pobj = ol->data;
        int16_t dx = pobj->x - p->x;
        if (dx < 34 && -74 < dx && (pobj->type < OTT_LEN || pobj->type == END_OBJ || pobj->type == SOLID_OBJ)){
            if ((uint16_t)((pobj->y + 40)/80) == linep){
                if (dx < 16 && -56 < dx){
                    is_in = 1;
                }
                oll = las;
                ol1 = ol;
            } else if (dx < 32 && -72 < dx) {
                if ((uint16_t)((pobj->y + 40)/80) == linep-1 && pobj->type != SCORE_OBJ){
                    ol2 = ol;
                } else if ((uint16_t)((pobj->y + 40)/80) == linep+1 && pobj->type != SCORE_OBJ){
                    ol3 = ol;
                }
            }
        }
        las = ol;
        ol = ol->next;
    }
    uint8_t on_ground = 0;
    if(speed_y <= 0.01 && ((p->y <= 41 && 39 <= p->y - speed_y) || (p->y <= 121 && 119 <= p->y - speed_y) || (p->y <= 201 && 199 <= p->y - speed_y))){
        on_ground = 1;
    }
    if (ol1 != NULL) pobj = ol1->data;
    if (ol1 == NULL || pobj->type == SCORE_OBJ) {
        if (on_ground){
            switch (movements & 0xf0) {
            case 0:
                if (movements & MOVE_UP){
                    if (linep != 0 && ol2 == NULL){
                        speed_y = Y_SPEED_UP;
                        *ch_movements = JUMP;
                    } else {
                        speed_x = 0;
                        *ch_movements = STOP;
                    }
                    movements_frame_cnt = 0;
                } else if (movements & MOVE_DOWN) {
                    if (linep != 2 && ol3 == NULL) {
                        p->y += 20;
                        speed_y = 0;
                        *ch_movements = IN_AIR;
                    } else {
                        speed_x = 0;
                        *ch_movements = STOP;
                    }
                    movements_frame_cnt = 0;
                } else if (movements & MOVE_FORWARD) {
                    if (movements_frame_cnt != 0){
                        movements_frame_cnt --;
                    } else {
                        *ch_movements = MOVING_FORWARD;
                    }
                } else if (movements & MOVE_BACK){
                    if (movements_frame_cnt != 0){
                        movements_frame_cnt --;
                    } else {
                        *ch_movements = MOVING_BACKWARD;
                    }
                } else {
                    if (movements_frame_cnt != 0){
                        movements_frame_cnt --;
                    } else {
                        *ch_movements = MOVING_FORWARD ? speed_x > 0 : MOVING_BACKWARD;
                    }
                }
                break;
            case DASH_M:
                if (movements_frame_cnt && (*ch_movements == CROUCH || *ch_movements == JUMP)) {
                    movements_frame_cnt --;
                } else {
                    *ch_movements = DASH;
                    movements_frame_cnt = 3 + TO_DASH_LEN*2;
                }
                break;
            case CROUCH_M:
                if (movements_frame_cnt && (*ch_movements == DASH || *ch_movements == JUMP)) {
                    movements_frame_cnt --;
                } else {
                    *ch_movements = CROUCH;
                    movements_frame_cnt = 15;
                }
                break;
            case JUMP_M:
                if (movements_frame_cnt && (*ch_movements == DASH || *ch_movements == CROUCH)) {
                    movements_frame_cnt --;
                } else {
                    *ch_movements = JUMP;
                    movements_frame_cnt = 0;
                    speed_y = Y_SPEED_JUMP;
                }
                break;
            }
        } else {
            *ch_movements = JUMP;
        }

        if(is_in && pobj->type == SCORE_OBJ){
            score += 1;
            remove_lt(oll);
        }
    } else {
        uint16_t match_move = (pobj->type == CROUCH_OBJ ? CROUCH_M : (pobj->type == JUMP_OBJ ? JUMP_M : DASH_M));
        uint16_t chh = (*ch_movements == CROUCH ? CROUCH_M : ((*ch_movements == JUMP || *ch_movements == IN_AIR) ? JUMP_M : (*ch_movements == DASH ? DASH_M : 0)));
        uint16_t is_match = ((movements | chh) & match_move) ? 1 : 0;
        is_match = is_match ? (on_ground || pobj->type == JUMP_OBJ) : 0;
        if (is_match && pobj->type != SOLID_OBJ && pobj->type != OBSTACLE) {
            if (movements & DASH_M) {
                movements_frame_cnt = 3 + TO_DASH_LEN*2;
            } else if (movements_frame_cnt > 3){
                --movements_frame_cnt;
            } else {
                movements_frame_cnt = 3;
            }
            *ch_movements = (pobj->type == CROUCH_OBJ ? CROUCH : (pobj->type == JUMP_OBJ ? JUMP : DASH));
            if (movements & JUMP_M) speed_y = Y_SPEED_JUMP;
        } else if (on_ground) {
            *ch_movements = MOVING_FORWARD ? speed_x > 0 : MOVING_BACKWARD;
            if (speed_x > 0) speed_x = 0;
            if ((!is_in && (movements & (MOVE_UP|MOVE_DOWN|JUMP_M))) || pobj->type == SOLID_OBJ || pobj->type == OBSTACLE) {
                if (movements & MOVE_UP){
                    if (linep != 0 && ol2 == NULL){
                        speed_y = Y_SPEED_UP;
                        *ch_movements = JUMP;
                    } else {
                        speed_x = 0;
                        *ch_movements = STOP;
                    }
                    movements_frame_cnt = 0;
                } else if (movements & MOVE_DOWN) {
                    if (linep != 2 && ol3 == NULL) {
                        p->y += 20;
                        speed_y = 0;
                        *ch_movements = IN_AIR;
                    } else {
                        speed_x = 0;
                        *ch_movements = STOP;
                    }
                    movements_frame_cnt = 0;
                } else if (movements & JUMP_M) {
                    if (movements_frame_cnt) {
                        movements_frame_cnt --;
                    } else {
                        *ch_movements = JUMP_M;
                        movements_frame_cnt = 0;
                        speed_y = Y_SPEED_JUMP;
                    }
                } else {
                    ;
                }
            } else {
                ;
            }
        } else {
            speed_x = 0;
            *ch_movements = JUMP;
        }
    }

    if (*ch_movements == MOVING_BACKWARD || *ch_movements == MOVING_FORWARD){
        if (-0.5 < speed_x && speed_x < 0.5) {
            *ch_movements = STOP;
        }
    }
    
    // #define MOVE_UP 0x01
    // #define MOVE_DOWN 0x02
    // #define MOVE_FORWARD 0x04
    // #define MOVE_BACK 0x08

    // #define DASH_M 0x10
    // #define CROUCH_M 0x20
    // #define JUMP_M 0x40

    // MOVING_FORWARD = 0,
    // MOVING_BACKWARD = 1,
    // STOP = 2,
    // CROUCH = 3,
    // DASH = 4,
    // JUMP = 5,
    // IN_AIR = 6
        
    // OBSTACLE,
    // SCORE_OBJ,
    // CROUCH_OBJ,
    // DASH_OBJ,
    // JUMP_OBJ,

    // OTT_LEN,

    // END_OBJ,
    // SOLID_OBJ,
}