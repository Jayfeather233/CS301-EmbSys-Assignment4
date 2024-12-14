#include "draw.h"
#include "objects.h"
#include "lcd.h"

#include <stddef.h>
#include <string.h>

#define FONT_SIZE 12

// uint16_t get_color(const uint16_t* color_book, const uint16_t* color_meta, uint16_t imx, uint16_t x, uint16_t y){
//     return color_meta[y*imx+x];
// }

int16_t mymin(int16_t a, int16_t b){
    return a<b ? a : b;
}
int16_t mymax(int16_t a, int16_t b){
    return a>b ? a : b;
}

void Fill_static_background(uint16_t color1, uint16_t color2){
    LCD_Set_Window(0, 0, lcddev.width, lcddev.height);
    LCD_SetCursor(0, 0);  // 设置光标位置
    LCD_WriteRAM_Prepare(); // 开始写入GRAM
    uint16_t ll1 = (lcddev.height>>1), ll2 = (lcddev.height>>3), ll3 = lcddev.width*2/3, ll4 = (lcddev.height >> 2);
    uint16_t dd0 = (lcddev.height>>1) + (lcddev.height>>2) - (lcddev.height>>3);
    uint16_t dd1 = (lcddev.height>>1) + (lcddev.height>>2);
    uint16_t dd2 = (lcddev.height>>1) + (lcddev.height>>2) + (lcddev.height>>3);
    for(uint16_t i=0;i<lcddev.height;++i){
        for(uint16_t j=0;j<lcddev.width;++j){
            if (i > dd2){
                LCD_WR_DATA(color2 - 0x1863);
            } else if (i > dd1){
                LCD_WR_DATA(color2 - 0x1042);
            } else if (i > dd0){
                LCD_WR_DATA(color2 - 0x821);
            } else if (i > ll1) {
                LCD_WR_DATA(color2);
            } else if (i>ll2 && j > ll3){
                LCD_WR_DATA(color2);
            } else if (j<ll3 && i > ll4) {
                if (((i+j)&0xf) && ((lcddev.width+i-j)&0xf)){
                    LCD_WR_DATA(color1);
                } else {
                    LCD_WR_DATA(color2);
                }
            } else if (i == ll4) {
                LCD_WR_DATA(color2);
            } else {
                LCD_WR_DATA(color1);
            }
        }
    }
}

void draw_elements(const myobj_t *p){
    const img_t * im = get_obj_img(p);
    if(p->x > lcddev.width || p->y > lcddev.height) return;
    if(p->x+im->img_x < 0 || p->y+im->img_y < 0) return;

    if (p->type == GRASS){
        if (im->scale == 1){
            int16_t dx = p->x, dy = p->y;
            for(uint16_t i=0;i<im->img_y;++i){
                uint16_t idx = i*im->img_x;
                for(uint16_t j=0;j<im->img_x;++j){
                    uint16_t idy = idx + j;
                    if(im->mask[idy>>4] & (1<<(idy&15))){
                        LCD_Fast_DrawPoint(dx,dy, im->data[i*im->img_x+j]);
                    }
                    dx += im->scale;
                    if (dx >= lcddev.width) break;
                }
                dx = p->x;
                dy += im->scale;
                if(dy >= lcddev.height) break;
            }
        } else if (im->scale == 2){
            int16_t dx = p->x, dy = p->y;
            uint16_t ddd;
            for(uint16_t i=0;i<im->img_y;++i){
                uint16_t idx = i*im->img_x;
                for(uint16_t j=0;j<im->img_x;++j){
                    uint16_t idy = idx + j;
                    if(im->mask[idy>>4] & (1<<(idy&15))){
                        ddd = im->data[i*im->img_x+j];
                        LCD_Fast_DrawPoint(dx,dy,ddd);
                        LCD_Fast_DrawPoint(dx+1,dy,ddd);
                        LCD_Fast_DrawPoint(dx,dy+1,ddd);
                        LCD_Fast_DrawPoint(dx+1,dy+1,ddd);
                    }
                    dx += im->scale;
                    if (dx >= lcddev.width) break;
                }
                dx = p->x;
                dy += im->scale;
                if(dy >= lcddev.height) break;
            }
        } else {
            int16_t dx = p->x, dy = p->y;
            for(uint16_t i=0;i<im->img_y;++i){
                uint16_t idx = i*im->img_x;
                for(uint16_t j=0;j<im->img_x;++j){
                    uint16_t idy = idx + j;
                    if(im->mask[idy>>4] & (1<<(idy&15))){
                        for(uint16_t ddx=0;ddx<im->scale;++ddx){
                            for(uint16_t ddy=0;ddy<im->scale;++ddy){
                                LCD_Fast_DrawPoint(dx+ddx, dy+ddy, im->data[i*im->img_x+j]);
                            }
                        }
                    }
                    dx += im->scale;
                    if (dx >= lcddev.width) break;
                }
                dx = p->x;
                dy += im->scale;
                if(dy >= lcddev.height) break;
            }
        }
    } else {
        int16_t las_y = 32000;
        int16_t dx = mymin((int16_t)lcddev.height, (int16_t)(p->y + im->img_y*im->scale)), dy = mymin((int16_t)lcddev.width, (int16_t)(p->x + im->img_x*im->scale));
        uint16_t i=0, j=0;
        uint16_t di=0, dj=0, ddi=p->y, ddj=p->x;
        int16_t ux = -p->x;

        if (p->y < 0.01) {
            int16_t uy = -p->y;
            di = uy%im->scale;
            i = uy/im->scale;
            ddi = 0;
        }
        for(int16_t ii=ddi;ii<dx;++ii){
            
            if (ux >= 0) {
                dj = ux%im->scale;
                j = ux/im->scale;
                ddj = 0;
            } else {
                dj = 0;
                j = 0;
                ddj = -ux;
            }
            uint16_t idx = i*im->img_x;
            las_y = 32000;
            for(uint16_t jj=ddj;jj<dy;++jj){
                uint16_t idy = idx + j;
                if(im->mask[idy>>4] & (1<<(idy&15))){
                    POINT_COLOR = im->data[i*im->img_x+j];
                    if (las_y + 1 != ii){
                        LCD_DrawPoint(jj,ii);
                        las_y = ii;
                    } else {
                        LCD_WR_DATA(POINT_COLOR);
                        ++ las_y;
                    }
                }
                if ((++dj) == im->scale) {
                    dj=0;
                    ++j;
                }
            }
            if ((++di) == im->scale) {
                di=0;
                ++i;
            }
        }
    }
}

void draw_elements_notrans(const myobj_t *p) {
    if (p->type == FIGURE || p->type == GRASS || p->type == STOP_UI || p->type == SCORE_OBJ || p->type == SOLID_OBJ || p->type == END_OBJ || p->type < OTT_LEN){
        // LCD_ShowString(20, 160, 300, 12, 12, "drawing ");
        // LCD_ShowNum(0, 160, p->type, 2, 12);
        draw_elements(p);
        return;
    }
    const img_t * im = get_obj_img(p);
    if(p->x >= lcddev.width || p->y >= lcddev.height) return;
    if(p->x+im->img_x <= 0 || p->y+im->img_y <= 0) return;
    
    int16_t height, width;
    int16_t npx = p->x, npy = p->y;
    uint16_t i, j;
    width = mymin(im->img_x * im->scale + npx, lcddev.width);
    height = mymin(im->img_y * im->scale + npy, lcddev.height); // 高度
    uint16_t ddx = npx < 0 ? 0 : npx;
    uint16_t ddy = npy < 0 ? 0 : npy;
    if (width <= 0 || height <= 0) return;
    if (ddx > width || ddy > height) {
        LCD_ShowString(20, 160, 300, 12, 12, "invalid drawing ");
        LCD_ShowNum(0, 160, p->type, 2, 12);
        return;
    }
    LCD_Set_Window(ddx, ddy, width-ddx, height-ddy);
    LCD_SetCursor(ddx, ddy);  // 设置光标位置
    LCD_WriteRAM_Prepare(); // 开始写入GRAM
    for (i = ddy; i < height; i++) {
        for (j = ddx; j < width; j++)
            LCD_WR_DATA(im->data[(j-npx)/im->scale + im->img_x * (i-npy)/im->scale]); // 写入数据
    }
    LCD_Set_Window(0, 0, lcddev.width, lcddev.height);
}


void draw_UI(const myobj_t *p){
    const img_t* im = get_obj_img(p);
    LCD_Fill_Window(mymax(p->x, 0), mymax(p->y, 0), mymin(p->x + im->img_x - 1, lcddev.width-1), mymin(p->y + im->img_y - 1, lcddev.height-1), im->data[0]);
    const char *ss = im->mask;
    size_t lss = strlen(ss);
    uint16_t dx = (im->img_x - lss * (FONT_SIZE>>1))>>1;
    uint16_t dy = (im->img_y - FONT_SIZE)>>1;
    LCD_ShowString_trans(p->x + dx, p->y + dy, lss * ((FONT_SIZE>>1)+1), FONT_SIZE, FONT_SIZE, ss);
}

void draw_UI_str(const myobj_t *p) {
    const img_t* im = get_obj_img(p);
    const char *ss = im->mask;
    size_t lss = strlen(ss);
    uint16_t dx = (im->img_x - lss * (FONT_SIZE>>1))>>1;
    uint16_t dy = (im->img_y - FONT_SIZE)>>1;
    LCD_ShowString_trans(p->x + dx, p->y + dy, lss * ((FONT_SIZE>>1)+1), FONT_SIZE, FONT_SIZE, ss);
}

void draw_color_on(uint16_t color, int16_t dx, int16_t dy, uint16_t datax, uint16_t datay, uint16_t *data){
    if(dx < 0 || dy < 0 || dx >= datax || dy >= datay){
        return;
    }
    data[dx+dy*datax] = color;
}

void draw_elements_on(const myobj_t *p, uint16_t shiftx, uint16_t shifty, uint16_t datax, uint16_t datay, uint16_t *data){
    const img_t * im = get_obj_img(p);
    if(p->x-shiftx > datax || p->y-shifty > datay) return;
    if(p->x-shiftx+im->img_x < 0 || p->y-shifty+im->img_y < 0) return;
    
    if (im->scale == 1){
        int16_t dx = p->x - shiftx, dy = p->y - shifty;
        for(uint16_t i=0;i<im->img_y;++i){
            uint16_t idx = i*im->img_x;
            for(uint16_t j=0;j<im->img_x;++j){
                uint16_t idy = idx + j;
                if(im->mask[idy>>4] & (1<<(idy&15))){
                    draw_color_on(im->data[i*im->img_x+j], dx, dy, datax, datay, data);
                }
                dx += im->scale;
                if (dx >= datax) break;
            }
            dx = p->x - shiftx;
            dy += im->scale;
            if(dy >= datay) break;
        }
    } else if (im->scale == 2){
        int16_t dx = p->x - shiftx, dy = p->y - shifty;
        uint16_t ddd;
        for(uint16_t i=0;i<im->img_y;++i){
            uint16_t idx = i*im->img_x;
            for(uint16_t j=0;j<im->img_x;++j){
                uint16_t idy = idx + j;
                if(im->mask[idy>>4] & (1<<(idy&15))){
                    ddd = im->data[i*im->img_x+j];
                    draw_color_on(ddd, dx, dy, datax, datay, data);
                    draw_color_on(ddd, dx+1, dy, datax, datay, data);
                    draw_color_on(ddd, dx, dy+1, datax, datay, data);
                    draw_color_on(ddd, dx+1, dy+1, datax, datay, data);
                }
                dx += im->scale;
                if (dx >= datax) break;
            }
            dx = p->x - shiftx;
            dy += im->scale;
            if(dy >= datay) break;
        }
    } else {
        int16_t dx = p->x - shiftx, dy = p->y - shifty;
        for(uint16_t i=0;i<im->img_y;++i){
            uint16_t idx = i*im->img_x;
            for(uint16_t j=0;j<im->img_x;++j){
                uint16_t idy = idx + j;
                if(im->mask[idy>>4] & (1<<(idy&15))){
                    for(uint16_t ddx=0;ddx<im->scale;++ddx){
                        for(uint16_t ddy=0;ddy<im->scale;++ddy){
                            draw_color_on(im->data[i*im->img_x+j], dx+ddx, dy+ddy, datax, datay, data);
                        }
                    }
                }
                dx += im->scale;
                if (dx >= datax) break;
            }
            dx = p->x - shiftx;
            dy += im->scale;
            if(dy >= datay) break;
        }
    }
}

uint8_t show_background = 1;
extern status_t character_movements;
void draw_all_FDP(const list_t *ol){
    if (show_background) Fill_static_background(0xAD55, 0x9CD3);
    // LCD_Fill_Window(0, 0, lcddev.width-1, lcddev.height-1, 0xAD55);
    uint16_t split_num = 8;
    if(lcddev.height % 6 == 0){
        split_num = 6;
    }
    const list_t *p = ol;
    size_t sz = sizeof(uint16_t)*lcddev.height*lcddev.width/split_num;
        
    while(p != NULL){
        update_obj(p->data, character_movements);
        if (((myobj_t*)(p->data))->type != BUTTON){
            draw_elements_notrans(p->data);
        }
        p=p->next;
    }
    p = ol;
    POINT_COLOR = RED;
    while(p != NULL){
        if (((myobj_t*)(p->data))->type == BUTTON){
            draw_UI(p->data);
        }
        p=p->next;
    }
}

void draw_all_Window(const list_t *ol){
    uint16_t split_num = 8;
    if(lcddev.height % 6 == 0){
        split_num = 6;
    }
    const list_t *p = ol;
    size_t sz = sizeof(uint16_t)*lcddev.height*lcddev.width/split_num;
    uint16_t *c_data = (uint16_t*)malloc(sz);
    if (c_data == NULL){
        show_warn("Unable to alloc display buffer.");
        return;
    }
    for(uint16_t i=0;i<split_num;++i){
        memset(c_data, 0xAD55, sz);
        uint16_t dty = lcddev.height/split_num;
        uint16_t sty = i*dty;
        
        while(p != NULL){
            if (((myobj_t*)(p->data))->type != BUTTON){
                draw_elements_on(p->data, 0, sty, lcddev.width, dty, c_data);
            }
            p=p->next;
        }
        p = ol;
        LCD_myColor_Fill_Window(0, sty, lcddev.width-1, sty+dty-1, c_data, 1);
    }
    free(c_data);
    while(p != NULL){
        if (((myobj_t*)(p->data))->type == BUTTON){
            draw_UI(p->data);
        }
        p=p->next;
    }
}

void draw_all(const list_t *ol){
    // draw_all_Window(ol);
    draw_all_FDP(ol);
}