#ifndef MAP_H
#define MAP_H

#include <engine/types.h>

typedef struct Map Map;

// Tile information for map creation.
typedef struct TilesInfo {
  Sprite *tile_sprites;  // Array of tile sprites
  uint32_t sprite_count; // Number of tile sprites
  uint32_t *tiles;       // 2D array (flattened) of tile sprite indices
  // Height of the tile sides in pixels. Must be correctly set for map size calculations.
  uint32_t sides_height;
} TilesInfo;

// Create map with given width and height (in tiles) and tiles info.
//
// Ownership of the given TilesInfo is transferred to the map!.
Map *map_create(uint32_t width, uint32_t height, TilesInfo ti);
void map_free(Map *map);

// Check that given point is within map boundaries, considering a margin.
//
// You can use that to ensure that objects are within the map area.
bool is_point_within_map(Map *map, VectorU32 pos, uint32_t margin);

// Generate a random position within the map boundaries, considering a margin.
VectorU32 map_gen_random_position(Map *map, uint32_t margin);

// Returns map size in pixels.
VectorU32 map_get_size(Map *map);

#endif
