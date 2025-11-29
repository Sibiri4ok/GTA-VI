#ifndef ENGINE_COORDINATES_H
#define ENGINE_COORDINATES_H

#include <engine/types.h>

// Convert isometric tile coordinates to world coordinates
// Accepts isometric x, y tile coordinates and map height (in tiles)
Vector tile_to_world(int x, int y, int map_height);

// Convert world coordinates to isometric tile coordinates
// Accepts world position and map height (in tiles)
Vector world_to_tile(float x, float y, int map_height);

#endif
