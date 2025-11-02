#ifndef RENDERER_H
#define RENDERER_H

#include "core/types.h"
#include <game/map.h>
#include <stdbool.h>
#include <stdint.h>

void draw_sprite(uint32_t *framebuffer,
    int fb_width,
    int fb_height,
    Sprite *sprite,
    int x,
    int y,
    bool flip_horizontal);

void render_frame_static(Map *map,
    uint32_t *framebuffer,
    int frame_width,
    int frame_height,
    Vector2 camera_pos);

#endif
