#ifndef ENGINE_COORDINATES_H
#define ENGINE_COORDINATES_H

#include <engine/types.h>

Vector iso_world_to_tile(Vector world_pos, int map_height);
Vector iso_tile_to_world(int x, int y, int map_height);

#endif
