#ifndef RENDERER_H
#define RENDERER_H

#include "core/types.h"
#include <stdbool.h>
#include <stdint.h>

// Sprite loading and management
Sprite load_sprite(const char *path);
Sprite load_sprite_scaled(const char *path, int scale);
void free_sprite(Sprite *sprite);

// Drawing functions
void draw_sprite(uint32_t *framebuffer,
    int fb_width,
    int fb_height,
    Sprite *sprite,
    int x,
    int y,
    bool flip_horizontal);

#endif
