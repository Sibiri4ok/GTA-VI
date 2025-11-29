#include "core/types_priv.h"
#include "graphics/alpha_blend.h"
#include "graphics/camera.h"
#include "graphics/render.h"
#include "random/random_priv.h"
#include "world/map_priv.h"
#include <engine/coordinates.h>
#include <engine/random.h>
#include <engine/types.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static bool map_is_valid_position(const Map *map, uint32_t x, uint32_t y);
static Sprite *map_get_tile(const Map *map, uint32_t x, uint32_t y);
static void map_render_tiles(Map *map);

// Create map with given tile width and height.
//
// Tiles are generated inside map_create. Pre-render tiles into buffer.
Map *map_create(uint32_t width, uint32_t height, TilesInfo ti) {
  if (!ti.tile_sprites || !ti.tiles || ti.sprite_count == 0) {
    if (ti.tiles) { free(ti.tiles); }
    if (ti.tile_sprites) { free_sprites(ti.tile_sprites, ti.sprite_count); }
    return NULL;
  }

  Map *map = (Map *)calloc(1, sizeof(Map));
  if (!map) {
    free(ti.tiles);
    free_sprites(ti.tile_sprites, ti.sprite_count);
    return NULL;
  }
  map->ti = ti;

  map->width = width;
  map->height = height;

  for (uint32_t i = 0; i < ti.sprite_count; i++) {
    Sprite sprite = ti.tile_sprites[i];
    if (i == 0) {
      if (sprite.height < ti.sides_height) { // tile sides height cannot be greater than sprite height
        map_free(map);
        return NULL;
      }
      map->tile_width = sprite.width;
      map->tile_height = sprite.height - ti.sides_height;
      continue;
    }
    if (sprite.width != map->tile_width || sprite.height != map->tile_height + ti.sides_height) {
      // All tile sprites must have the same dimensions
      map_free(map);
      return NULL;
    }
  }

  map->width_pix = (width + height) * (map->tile_width / 2);
  map->height_pix = (width + height) * (map->tile_height / 2) + ti.sides_height;

  map->pixels = (uint32_t *)calloc(map->width_pix * map->height_pix, sizeof(uint32_t));
  if (!map->pixels) {
    map_free(map);
    return NULL;
  }

  map_render_tiles(map);

  return map;
}

void map_free(Map *map) {
  if (!map) return;

  if (map->pixels) { free(map->pixels); }
  free_sprites(map->ti.tile_sprites, map->ti.sprite_count);
  if (map->ti.tiles) { free(map->ti.tiles); }
  free(map);
}

VectorU32 map_get_size(Map *map) {
  VectorU32 size = {0, 0};
  if (!map) return size;

  size.x = (uint32_t)map->width_pix;
  size.y = (uint32_t)map->height_pix;
  return size;
}

inline static bool map_is_valid_position(const Map *map, uint32_t x, uint32_t y) {
  if (!map) return false;
  return (x < map->width && y < map->height);
}

inline static Sprite *map_get_tile(const Map *map, uint32_t x, uint32_t y) {
  if (!map || !map->ti.tiles) return NULL;

  if (map_is_valid_position(map, x, y)) { return &map->ti.tile_sprites[map->ti.tiles[y * map->width + x]]; }
  return NULL;
}

static void map_render_tiles(Map *map) {
  if (!map || !map->ti.tiles) return;

  // Tile coordinates
  for (uint32_t yy = 0; yy < map->height; yy++) {
    for (uint32_t xx = 0; xx < map->width; xx++) {
      Sprite *sprite = map_get_tile(map, xx, yy);
      if (!sprite) continue;

      if (!sprite->pixels) continue;

      Vector world_pos = tile_to_world(map, xx, yy);

      int map_x = (int)world_pos.x;
      int map_y = (int)world_pos.y;

      // Draw tiles
      for (uint32_t sy = 0; sy < sprite->height; sy++) {
        for (uint32_t sx = 0; sx < sprite->width; sx++) {
          uint32_t px = map_x + sx;
          uint32_t py = map_y + sy;

          // Bounds check
          if (px >= map->width_pix || py >= map->height_pix) { continue; }

          uint32_t src = sprite->pixels[sy * sprite->width + sx];
          int idx = py * map->width_pix + px;
          uint32_t dst = map->pixels[idx];
          map->pixels[idx] = alpha_blend(src, dst);
        }
      }
    }
  }
}

static float triArea(Vector p1, Vector p2, Vector p3) {
  return fabs((p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x)) / 2.0f;
}

typedef struct {
  // top, bottom, left, right corners of the quadrangle
  Vector t, b, l, r;
} MapCorners;

static MapCorners get_map_corners(Map *map, uint32_t margin) {
  MapCorners corners;
  corners.t = tile_to_world(map, 0, 0);
  corners.b = tile_to_world(map, map->width, map->height);
  corners.l = tile_to_world(map, 0, map->height);
  corners.r = tile_to_world(map, map->width, 0);
  corners.t.y += margin;
  corners.b.y -= margin;
  corners.l.x += margin;
  corners.r.x -= margin;
  return corners;
}

static bool pointInQuadrangle(Vector a, Vector b, Vector c, Vector d, Vector p) {
  float s1 = triArea(a, b, p);
  float s2 = triArea(b, c, p);
  float s3 = triArea(c, d, p);
  float s4 = triArea(d, a, p);

  float S = triArea(a, b, c) + triArea(a, c, d);

  float sum = s1 + s2 + s3 + s4;

  return fabs(sum - S) < 1e-3;
}

bool is_point_within_map(Map *map, VectorU32 pos, uint32_t margin) {
  MapCorners mc = get_map_corners(map, margin);
  return pointInQuadrangle(mc.t, mc.r, mc.b, mc.l, (Vector){(float)pos.x, (float)pos.y});
}

VectorU32 map_gen_random_position(Map *map, uint32_t margin) {
  Vector center = tile_to_world(map, map->width / 2, map->height / 2);
  VectorU32 pos = {(uint32_t)center.x, (uint32_t)center.y};
  if (!map || margin >= (map->width_pix / 2) || margin >= (map->height_pix / 2)) return pos;

  MapCorners mc = get_map_corners(map, margin);

  uint32_t range_x = mc.r.x - mc.l.x;
  uint32_t range_y = mc.b.y - mc.t.y;
  for (int i = 0; i < 100; i++) {
    Vector p = {mc.l.x + (rand_big() % range_x), mc.t.y + (rand_big() % range_y)};

    if (pointInQuadrangle(mc.t, mc.r, mc.b, mc.l, p)) {
      pos.x = (uint32_t)p.x;
      pos.y = (uint32_t)p.y;
      return pos;
    }
  }

  return pos;
}
