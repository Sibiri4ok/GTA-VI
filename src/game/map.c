#include "game/map.h"
#include "graphics/alpha_blend.h"
#include "graphics/coordinates.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

static uint32_t hash_u32(int x, int y);
static float rand01(int x, int y);
static bool map_is_valid_position(const Map *map, int x, int y);
static TileType map_get_tile(const Map *map, int x, int y);
static void map_render(Map *map);
static void generate_all_tiles(Map *map);
static Sprite generate_tile(Color base, TileType type);

static uint32_t hash_u32(int x, int y) {
  uint32_t h = (uint32_t)(x * 374761393u + y * 668265263u);
  h = (h ^ (h >> 13)) * 1274126177u;
  return h ^ (h >> 16);
}

static float rand01(int x, int y) {
  return (hash_u32(x, y) & 0xFFFF) / 65535.0f;
}

Map *map_create(int width, int height) {
  Map *map = (Map *)calloc(1, sizeof(Map));
  if (!map) { return NULL; }

  map->width = width;
  map->height = height;

  map->width_pix = (width + height) * (ISO_TILE_WIDTH / 2);
  map->height_pix = (width + height) * (ISO_TILE_HEIGHT / 2);

  map->pixels = (uint32_t *)calloc(map->width_pix * map->height_pix, sizeof(uint32_t));
  if (!map->pixels) {
    free(map);
    return NULL;
  }

  map->tiles = (TileType *)calloc(width * height, sizeof(TileType));
  if (!map->tiles) {
    free(map->pixels);
    free(map);
    return NULL;
  }

  for (int i = 0; i < width * height; i++) { map->tiles[i] = TILE_GRASS; }

  generate_all_tiles(map);
  map_render(map);

  return map;
}

void map_destroy(Map *map) {
  if (!map) return;

  for (int i = 0; i < TILE_MAX; i++) {
    if (map->tile_sprites[i].pixels) {
      free(map->tile_sprites[i].pixels);
      map->tile_sprites[i].pixels = NULL;
    }
  }

  if (map->tiles) { free(map->tiles); }
  if (map->pixels) { free(map->pixels); }

  free(map);
}

static bool map_is_valid_position(const Map *map, int x, int y) {
  if (!map) return false;
  return (x >= 0 && x < map->width && y >= 0 && y < map->height);
}

static TileType map_get_tile(const Map *map, int x, int y) {
  if (!map || !map->tiles) return TILE_EMPTY;

  if (map_is_valid_position(map, x, y)) { return map->tiles[y * map->width + x]; }
  return TILE_EMPTY;
}

static void map_render(Map *map) {
  if (!map || !map->tiles) return;

  // Tile coordinates
  for (int yy = 0; yy < map->height; yy++) {
    for (int xx = 0; xx < map->width; xx++) {
      TileType tile = map_get_tile(map, xx, yy);
      if (tile == TILE_EMPTY) continue;

      Sprite *sprite = &map->tile_sprites[tile];
      if (!sprite->pixels) continue;

      Vector2 world_pos = iso_tile_to_world(xx, yy);

      // Convert world coordinates so that x is not negative
      int offset_x = map->height * (ISO_TILE_WIDTH / 2);
      int map_x = (int)(world_pos.x + offset_x - (ISO_TILE_WIDTH / 2));
      int map_y = (int)world_pos.y;

      // Draw tiles
      for (int sy = 0; sy < sprite->height; sy++) {
        for (int sx = 0; sx < sprite->width; sx++) {
          int px = map_x + sx;
          int py = map_y + sy;

          // Bounds check
          if (px < 0 || px >= map->width_pix || py < 0 || py >= map->height_pix) { continue; }

          uint32_t src = sprite->pixels[sy * sprite->width + sx];
          int idx = py * map->width_pix + px;
          uint32_t dst = map->pixels[idx];
          map->pixels[idx] = alpha_blend(src, dst);
        }
      }
    }
  }
}

static void generate_all_tiles(Map *map) {
  if (!map) return;

  for (int i = 0; i < TILE_MAX; i++) {
    Color color = (Color){255, 128, 128, 128};
    switch (i) {
    case TILE_GROUND: color = (Color){255, 139, 90, 60}; break;
    case TILE_SHADOW: color = (Color){120, 0, 0, 0}; break;
    case TILE_WATER: color = (Color){255, 30, 120, 200}; break;
    case TILE_SAND: color = (Color){255, 235, 210, 160}; break;
    case TILE_ROCK: color = (Color){255, 140, 140, 150}; break;
    case TILE_GRASS: color = (Color){255, 90, 155, 85}; break;
    default: break;
    }

    map->tile_sprites[i] = generate_tile(color, (TileType)i);
  }
}

static Sprite generate_tile(Color base, TileType type) {
  const int w = ISO_TILE_WIDTH;
  const int h = ISO_TILE_HEIGHT;

  Sprite sprite;
  sprite.width = w;
  sprite.height = h;
  sprite.pixels = (uint32_t *)calloc(w * h, sizeof(uint32_t));

  if (!sprite.pixels) {
    sprite.width = 0;
    sprite.height = 0;
    return sprite;
  }

  const float half_w = w * 0.5f;
  const float half_h = h * 0.5f;

  for (int y = 0; y < h; y++) {
    float t = fabsf(y - half_h) / half_h;
    float extent = (1.0f - t) * half_w;
    int x_min = (int)floorf(half_w - extent);
    int x_max = (int)ceilf(half_w + extent);

    for (int x = 0; x < w; x++) {
      int idx = y * w + x;

      if (x < x_min || x > x_max) {
        sprite.pixels[idx] = 0x00000000; // Transparent
        continue;
      }

      float r = (float)base.r;
      float g = (float)base.g;
      float b = (float)base.b;
      float a = (float)base.a;

      float n = rand01(x, y) * 0.15f - 0.075f; // [-0.075, 0.075]

      switch (type) {
      case TILE_GRASS: {
        float shade = 0.92f + rand01(x * 3, y * 5) * 0.16f; // [0.92, 1.08]
        r = r * shade;
        g = g * (shade + 0.05f);
        b = b * (shade - 0.02f);
        if ((hash_u32(x, y) % 97u) == 0u) g = fminf(g + 25.0f, 255.0f);
      } break;
      case TILE_WATER: {
        float gy = (float)y / (float)h;
        float wave = 0.04f * sinf((x + y * 0.5f) * 0.2f);
        r = 20.0f + 10.0f * gy;
        g = 100.0f + 40.0f * gy;
        b = 180.0f + 50.0f * gy;
        r *= 1.0f + wave;
        g *= 1.0f + wave;
        b *= 1.0f + wave;
      } break;
      case TILE_SAND: {
        float speck = rand01(x * 7, y * 11) * 0.25f - 0.125f;
        r *= 1.0f + speck;
        g *= 1.0f + speck * 0.5f;
        b *= 1.0f - speck * 0.3f;
      } break;
      case TILE_ROCK: {
        float rough = rand01(x * 5, y * 5) * 0.3f - 0.15f;
        r *= 1.0f + rough;
        g *= 1.0f + rough;
        b *= 1.0f + rough;
      } break;
      case TILE_SHADOW: {
        r = 0.0f;
        g = 0.0f;
        b = 0.0f;
        a = 120.0f;
      } break;
      default: {
        r *= 1.0f + n;
        g *= 1.0f + n;
        b *= 1.0f + n;
      } break;
      }

      // Edge shading
      float edge = fminf(fabsf(x - half_w) / fmaxf(extent, 1.0f), t);
      float shade = 1.0f - 0.15f * fminf(edge * 1.5f, 1.0f);
      r *= shade;
      g *= shade;
      b *= shade;

      // Clamp and convert to ARGB format
      uint8_t A = (uint8_t)fminf(fmaxf(a, 0.0f), 255.0f);
      uint8_t R = (uint8_t)fminf(fmaxf(r, 0.0f), 255.0f);
      uint8_t G = (uint8_t)fminf(fmaxf(g, 0.0f), 255.0f);
      uint8_t B = (uint8_t)fminf(fmaxf(b, 0.0f), 255.0f);

      sprite.pixels[idx] = (A << 24) | (R << 16) | (G << 8) | B;
    }
  }

  return sprite;
}
