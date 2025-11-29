#ifndef ENGINE_COORDINATES_H
#define ENGINE_COORDINATES_H

#include <engine/map.h>
#include <engine/types.h>

// Convert isometric tile coordinates to world coordinates.
// Accepts map and x, y tile coordinates.
Vector tile_to_world(Map *map, int x, int y);

// Convert world coordinates to isometric tile coordinates.
// Accepts map and world position.
Vector world_to_tile(Map *map, float x, float y);

#endif
