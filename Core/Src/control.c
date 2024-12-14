#include "control.h"
#include "lcd.h"

#include <stdlib.h>

const float MAX_X_SPEED = 10 * LOGIC_SKIP;
const float X_SPEED_DEC = 0.3 * LOGIC_SKIP;
const float X_SPEED_DELTA = 5 * LOGIC_SKIP;

const float Y_SPEED_JUMP = 7.0 * SQ_LOGIC_SKIP;
const float Y_SPEED_UP = 10.5 * SQ_LOGIC_SKIP;
const float Y_SPEED_DEC = 0.5 * LOGIC_SKIP;

uint16_t movements = 0;
uint16_t movements_buffer;

status_t character_movements = STOP;

static uint16_t st_movx, st_movy, las_movx, las_movy;
static uint8_t flag;

extern float speed_x, speed_y;
extern float total_x, end_position;

extern uint8_t sig_end;

uint16_t find_button(uint16_t x, uint16_t y, const list_t *ol){
    const list_t *pp = ol;
    while (pp != NULL) {
        if (((myobj_t *)(pp->data))->type == BUTTON) {
            const myobj_t *pw = pp->data;
            const img_t * im = get_obj_img(pw);
            if (pw->x-0.01 <= x && x < pw->x+0.01 + im->img_x && pw->y-0.01 <= y &&
                y < pw->y+0.01 + im->img_y) {
                return (uint16_t)(im->data[1]);
            }
        } else {
            break;
        }
        pp = pp->next;
    }
    return 0;
}

void update_movement(const _m_tp_dev *p, const list_t *ol)
{
    if (p->sta & TP_PRES_DOWN) {
        if (p->x[0] < lcddev.width && p->y[0] < lcddev.height) {
            if (flag == 0) {
                uint16_t ww = find_button(p->x[0], p->y[0], ol);
                if (ww != 0) {
                    movements |= ww;
                    return;
                }
                st_movx = p->x[0];
                st_movy = p->y[0];
                flag = 1;
            }
            else if (flag >= 1) {
                las_movx = p->x[0];
                las_movy = p->y[0];
                flag = 2;
            } else {
                flag = 0;
            }
        }
    }
    else {
        if (flag == 2) {
            int16_t dx = (las_movx - st_movx)*2;
            int16_t dy = las_movy - st_movy;

            if (-40 <= dx && dx <= 40 && -20 <= dy && dy <= 20) {
                movements |= find_button((las_movx+st_movx)>>1, (las_movy+st_movy)>>1, ol);
                flag = 0;
            } else if (dx >= dy && dx >= -dy){
                movements |= MOVE_FORWARD;
            } else if (dx <= dy && dx <= -dy){
                movements |= MOVE_BACK;
            } else if (dy >= dx && dy >= -dx){
                movements |= MOVE_DOWN;
            } else {
                movements |= MOVE_UP;
            }
        } else if (flag == 1) {
            movements |= find_button(st_movx, st_movy, ol);
        }
        flag = 0;
    }
}
void update_elements(list_t *ol)
{
    // LCD_ShowNum(20, 200, movements, 5, 12);
    movements_buffer = movements;
    movements = 0;
    if ((movements_buffer & MOVE_UP) && (movements_buffer & MOVE_DOWN)) {
        movements_buffer &= 0xfc;
    }
    if ((movements_buffer & MOVE_FORWARD) && (movements_buffer & MOVE_BACK)) {
        movements_buffer &= 0xf3;
    }
    if ((movements_buffer & 0xf0) != DASH_M && (movements_buffer & 0xf0) != CROUCH_M && (movements_buffer & 0xf0) != JUMP_M) {
        movements_buffer &= 0x0f;
    }
    list_t *p = ol, *las = ol;
    while (p != NULL) {
        update_element(p->data, las);
        las = p;
        p=p->next;
    }
    if(total_x < end_position+0.01) total_x += speed_x;
    update_speed();
    // movements = 0;
}
static float figure_yy;
void update_element(myobj_t *o, list_t *lasp) {
    if(o->type == FIGURE){
        get_next_status(o, movements_buffer, &character_movements, obstacle_list_head->next);
        if(total_x > end_position-0.01){
            o->x += speed_x;
            if (o->x > 280){
                sig_end = 1;
            }
        }
        if(speed_y <= 0){
            if (o->y < 41 && 39 < o->y - speed_y){
                o->y = 40;
                speed_y = 0;
            } else if (o->y < 121 && 119 < o->y - speed_y){
                o->y = 120;
                speed_y = 0;
            } else if (o->y < 201 && 199 < o->y - speed_y){
                o->y = 200;
                speed_y = 0;
            }
        }
        if(character_movements == IN_AIR || character_movements == JUMP || speed_y > 0.01) o->y -= speed_y;
        figure_yy = o->y;
    } else if (o->type == GROUND){
        if(total_x < end_position+0.01)
            o->x -= speed_x;
        if (o->x < -320){
            o->x += 640;
        } else if (o->x > 320) {
            o->x -= 640;
        }
    } else {
        if(o->type != STOP_UI){
            if(total_x < end_position+0.01)
                o->x -= speed_x;
        }
    }

    if(-0.5 < speed_x && speed_x < 0.5){
        list_t *p = NULL;
        if(character_list_head->next != NULL) p = character_list_head->next->next;
        if(p == NULL || ((myobj_t*)(p->data))->type != STOP_UI){
            p = character_list_head->next;
            insert_lt(p, new_myobj(((myobj_t*)(p->data))->x+16,((myobj_t*)(p->data))->y-20, STOP_UI));
        } else {
            myobj_t *uu = character_list_head->next->data;
            myobj_t *uu2 = p->data;
            uu2->x = uu->x+16;
            uu2->y = uu->y-20;
        }
    } else {
        if(character_list_head->next != NULL && character_list_head->next->next != NULL){
            if(((myobj_t*)(character_list_head->next->next->data))->type == STOP_UI){
                remove_lt(character_list_head->next);
            }
        }
    }

    if (o->x + o->data->img_x < -30) {
        remove_lt(lasp);
    }
}

extern uint16_t movements_frame_cnt;
void update_speed() {
    if(speed_x > X_SPEED_DEC) speed_x -= X_SPEED_DEC;
    else if (speed_x < -X_SPEED_DEC) speed_x += X_SPEED_DEC;
    else speed_x = 0;
    speed_y -= Y_SPEED_DEC;

    if (movements_buffer & MOVE_FORWARD){
        if (character_movements == CROUCH){
            speed_x += X_SPEED_DELTA * CROUCH_FRAC;
        } else {
            speed_x += X_SPEED_DELTA;
        }
    }
    if (movements_buffer & MOVE_BACK) {
        if (character_movements == CROUCH){
            speed_x -= X_SPEED_DELTA * CROUCH_FRAC;
        } else {
            speed_x -= X_SPEED_DELTA;
        }
    }

    if (character_movements == DASH && movements_frame_cnt <= 3){
        speed_x = MAX_X_SPEED;
    }
    
    if (character_movements == CROUCH){
        if(speed_x > MAX_X_SPEED * CROUCH_FRAC){
            speed_x = MAX_X_SPEED * CROUCH_FRAC;
        } else if(speed_x < -MAX_X_SPEED * CROUCH_FRAC){
            speed_x = -MAX_X_SPEED * CROUCH_FRAC;
        }
    } else {
        if(speed_x > MAX_X_SPEED){
            speed_x = MAX_X_SPEED;
        } else if(speed_x < -MAX_X_SPEED){
            speed_x = -MAX_X_SPEED;
        }
    }
}

static uint8_t show_end_obj = 0;
void game_logic(list_t *ol){
    
    if (show_end_obj) return;
    list_t * last = ol;
    uint8_t free_rows = 0;
    uint8_t grass_rows = 0;
    uint8_t obst_rows = 0;
    while(ol != NULL){
        myobj_t *myo = ol->data;
        if (myo->x>190 && myo->type < OTT_LEN){
            if (myo->y < 48){
                free_rows |= 1;
            } else if(myo->y < 128){
                free_rows |= 2;
            } else {
                free_rows |= 4;
            }
        }
        if (myo->x>250 && myo->type == GRASS){
            if (myo->y < 48){
                grass_rows |= 1;
            } else if(myo->y < 128){
                grass_rows |= 2;
            } else {
                grass_rows |= 4;
            }
        }
        if (myo->x>120 && myo->type == OBSTACLE){
            if (myo->y < 48){
                obst_rows |= 1;
            } else if(myo->y < 128){
                obst_rows |= 2;
            } else {
                obst_rows |= 4;
            }
        }
        last = ol;
        ol = ol->next;
    }

    if (total_x > end_position-72){
        if(free_rows == 0){
            insert_lt(last, new_myobj(350, 0, SOLID_OBJ));
            insert_lt(last, new_myobj(350, 80, END_OBJ));
            insert_lt(last, new_myobj(350, 160, SOLID_OBJ));
            end_position += 30;
            show_end_obj = 1;
        } else {
            end_position = total_x+70;
        }
    } else if (free_rows != 0x7){
        uint8_t nfree = 0;
        if(free_rows&1){
            nfree ++;
        }
        if(free_rows&2){
            nfree ++;
        }
        if(free_rows&4){
            nfree ++;
        }
        int32_t tt = nfree*nfree*5 + 5;
        if (rand()%tt==0) {
            uint16_t line = rand()%3;
            while(free_rows & (1<<line)){
                line = (line+1)%3;
            }
            object_type_t ott = rand()%OTT_LEN;
            if(ott != OBSTACLE || (obst_rows | (1<<line)) != 0x7){
                insert_lt(last, new_myobj(320, 80*line, ott));
            }
        }
    }
    if (grass_rows != 0x7){
        int32_t tt = 5;
        if (rand()%tt==0) {
            uint16_t line = rand()%3;
            while(grass_rows & (1<<line)){
                line = (line+1)%3;
            }
            object_type_t ott = GRASS;
            insert_lt(last, new_myobj(320, 80*line, ott));
        }
    }
}