#ifndef COORDINATES_H
#define COORDINATES_H

#include <core/types.h>

Vector2 iso_world_to_tile(Vector2 world_pos);
Vector2 iso_tile_to_world(int x, int y);

#endif
