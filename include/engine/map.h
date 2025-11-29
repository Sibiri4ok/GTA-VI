#ifndef MAP_H
#define MAP_H

#include <engine/types.h>

typedef struct Map Map;

// Create map with given tile width and height.
Map *map_create(uint32_t width, uint32_t height);
void map_free(Map *map);

// Check that given point is within map boundaries, considering a margin.
bool is_point_within_map(const Map *map, VectorU32 pos, uint32_t margin);

// Generate a random position within the map boundaries, considering a margin.
VectorU32 map_gen_random_position(const Map *map, uint32_t margin);

// Returns map size in pixels.
VectorU32 map_get_size(const Map *map);

#endif
