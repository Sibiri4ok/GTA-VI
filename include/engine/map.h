#ifndef MAP_H
#define MAP_H

#include <engine/types.h>

typedef struct Map Map;

Map *map_create(int width, int height);
void map_free(Map *map);

VectorU32 map_gen_random_position(const Map *map, uint32_t margin);
VectorU32 map_get_size(const Map *map);
uint32_t map_get_pixel(const Map *map, uint32_t x, uint32_t y);

#endif
