/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include "graphic.h"
#include <string.h>

static void point_or(graphic_t *graphic, int index, unsigned char mask)
{
    graphic->buf[index] |= mask;
}

static void point_and(graphic_t *graphic, int index, unsigned char mask)
{
    graphic->buf[index] &= ~mask;
}

static void point_xor(graphic_t *graphic, int index, unsigned char mask)
{
    graphic->buf[index] ^= mask;
}

void graphic_init(graphic_t *graphic, int width, int height, unsigned char *buf)
{
    graphic->width = width;
    graphic->height = height;
    graphic->buf = buf;
    graphic->draw_logic = point_or;
}

void graphic_fill(graphic_t *graphic, unsigned char data)
{
    memset(graphic->buf, data, (graphic->width * graphic->height) / 8);
}

void graphic_set_logic(graphic_t *graphic, graphic_logic_t logic)
{
    switch (logic)
    {
    case GRAPHIC_LOGIC_OR:
        graphic->draw_logic = point_or;
        break;
    case GRAPHIC_LOGIC_AND:
        graphic->draw_logic = point_and;
        break;
    case GRAPHIC_LOGIC_XOR:
        graphic->draw_logic = point_xor;
        break;
    default:
        break;
    }
}

void graphic_draw_point(graphic_t *graphic, point_t *point)
{
    if (point->x < 0 || point->x >= graphic->width || point->y < 0 || point->y >= graphic->height)
        return;

    int index = (point->y / 8) * graphic->width + point->x;
    graphic->draw_logic(graphic, index, 1 << (point->y % 8));
}

void graphic_draw_v_line(graphic_t *graphic, point_t *start, int len)
{
    if (start->x < 0 || start->x >= graphic->width)
        return;

    int y = start->y;
    if (len < 0)
    {
        y += len;
        len = -len;
    }

    if (y < 0)
    {
        if (y + len < 0)
            return;

        len += y;
        y = 0;
    }
    else if (y >= graphic->height)
    {
        return;
    }

    int index = (y / 8) * graphic->width + start->x;
    int pos = y % 8;
    unsigned char mask = 0xff << pos;

    graphic->draw_logic(graphic, index, mask);
    len -= (8 - pos);
    index += graphic->width;
    mask = 0xff;
    len = (y + len >= graphic->height) ? graphic->height - y : len;

    while (len >= 8)
    {
        graphic->draw_logic(graphic, index, mask);
        index += graphic->width;
        len -= 8;
    }

    if (len > 0)
    {
        mask = 0xff >> (8 - len);
        graphic->draw_logic(graphic, index, mask);
    }
}

void graphic_draw_h_line(graphic_t *graphic, point_t *start, int len)
{
    if (start->y < 0 || start->y >= graphic->height)
        return;

    int x = start->x;
    if (len < 0)
    {
        x += len;
        len = -len;
    }

    if (x < 0)
    {
        if (x + len < 0)
            return;

        len += x;
        x = 0;
    }
    else if (x >= graphic->width)
    {
        return;
    }

    int index = (start->y / 8) * graphic->width + x;
    unsigned char mask = 1 << (start->y % 8);

    len = (x + len >= graphic->width) ? graphic->width - x : len;

    while (len > 0)
    {
        graphic->draw_logic(graphic, index, mask);
        index++;
        len--;
    }
}

void graphic_draw_line(graphic_t *graphic, point_t *start, point_t *end)
{
    int dx = end->x - start->x;
    int dy = end->y - start->y;
    int ux = ((dx > 0) << 1) - 1;
    int uy = ((dy > 0) << 1) - 1;
    int x = start->x, y = start->y, eps;

    dx = dx < 0 ? -dx : dx;
    dy = dy < 0 ? -dy : dy;

    if (dx > dy)
    {
        eps = dx / 2;
        for (x = start->x; x != end->x; x += ux)
        {
            graphic_draw_point(graphic, &(point_t){x, y});
            eps += dy;
            if (eps > dx)
            {
                eps -= dx;
                y += uy;
            }
        }
    }
    else
    {
        eps = dy / 2;
        for (y = start->y; y != end->y; y += uy)
        {
            graphic_draw_point(graphic, &(point_t){x, y});
            eps += dx;
            if (eps > dy)
            {
                eps -= dy;
                x += ux;
            }
        }
    }
    graphic_draw_point(graphic, &(point_t){x, y});
}

void graphic_draw_rect(graphic_t *graphic, rect_t *rect)
{
    point_t start = {rect->x, rect->y};
    graphic_draw_h_line(graphic, &start, rect->width);
    start.y++;
    graphic_draw_v_line(graphic, &start, rect->height - 1);
    start.x += rect->width - 1;
    graphic_draw_v_line(graphic, &start, rect->height - 1);
    start.y += rect->height - 2;
    start.x = rect->x + 1;
    graphic_draw_h_line(graphic, &start, rect->width - 2);
}

void graphic_fill_rect(graphic_t *graphic, rect_t *rect)
{
    point_t start = {rect->x, rect->y};
    for (int i = 0; i < rect->height; i++)
    {
        graphic_draw_h_line(graphic, &start, rect->width);
        start.y++;
    }
}
