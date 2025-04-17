/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __GRAPHIC_H__
#define __GRAPHIC_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _point
{
    int x;
    int y;
}point_t;

typedef struct _rect
{
    int x;
    int y;
    int width;
    int height;
}rect_t;

typedef struct _graphic
{
    int width;
    int height;
    unsigned char *buf;
    void (*draw_logic)(struct _graphic*, int, unsigned char);
}graphic_t;

typedef enum _graphic_logic
{
    GRAPHIC_LOGIC_OR,
    GRAPHIC_LOGIC_AND,
    GRAPHIC_LOGIC_XOR,
}graphic_logic_t;

void graphic_init(graphic_t *graphic, int width, int height, unsigned char *buf);
void graphic_fill(graphic_t *graphic, unsigned char data);
void graphic_set_logic(graphic_t *graphic, graphic_logic_t logic);
void graphic_draw_point(graphic_t *graphic, point_t *point);
void graphic_draw_v_line(graphic_t *graphic, point_t *start, int len);
void graphic_draw_h_line(graphic_t *graphic, point_t *start, int len);
void graphic_draw_line(graphic_t *graphic, point_t *start, point_t *end);
void graphic_draw_rect(graphic_t *graphic, rect_t *rect);
void graphic_fill_rect(graphic_t *graphic, rect_t *rect);
void graphic_draw_circle(graphic_t *graphic, point_t *center, int radius);
void graphic_fill_circle(graphic_t *graphic, point_t *center, int radius);

#ifdef __cplusplus
}
#endif

#endif
