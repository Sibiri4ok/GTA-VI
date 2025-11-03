#ifndef LOAD_SPRITE_H
#define LOAD_SPRITE_H

#include <core/types.h>
#include <stdint.h>

Sprite load_sprite(const char *path, float scale);
void free_sprite(Sprite *sprite);

#endif
