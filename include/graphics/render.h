#ifndef RENDERER_H
#define RENDERER_H

#include "core/types.h"
#include <stdbool.h>
#include <stdint.h>

// Drawing functions
void draw_sprite(uint32_t *framebuffer,
    int fb_width,
    int fb_height,
    Sprite *sprite,
    int x,
    int y,
    bool flip_horizontal);

#endif
