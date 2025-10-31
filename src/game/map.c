#include "game/map.h"
#include "graphics/camera.h"
#include "graphics/renderer.h"
#include <math.h>
#include <stdint.h>

static uint32_t hash_u32(int x, int y) {
  uint32_t h = (uint32_t)(x * 374761393u + y * 668265263u);
  h = (h ^ (h >> 13)) * 1274126177u;
  return h ^ (h >> 16);
}

static float rand01(int x, int y) {
  return (hash_u32(x, y) & 0xFFFF) / 65535.0f;
}

static SDL_Surface *create_isometric_tile_surface(Color base, TileType type) {
  const int w = ISO_TILE_WIDTH;
  const int h = ISO_TILE_HEIGHT;
  SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
  if (!surface) return NULL;

  SDL_LockSurface(surface);
  uint8_t *pixels = (uint8_t *)surface->pixels;
  const int pitch = surface->pitch;

  const float half_w = w * 0.5f;
  const float half_h = h * 0.5f;

  for (int yy = 0; yy < h; yy++) {
    float t = fabsf(yy - half_h) / half_h;
    float extent = (1.0f - t) * half_w;
    int x_min = (int)floorf(half_w - extent);
    int x_max = (int)ceilf(half_w + extent);
    uint32_t *row = (uint32_t *)(pixels + yy * pitch);

    for (int xx = 0; xx < w; xx++) {
      if (xx < x_min || xx > x_max) {
        row[xx] = 0x00000000; // прозрачный фон
        continue;
      }

      float r = (float)base.r;
      float g = (float)base.g;
      float b = (float)base.b;
      float a = (float)base.a;

      float n = rand01(xx, yy) * 0.15f - 0.075f;

      switch (type) {
      case TILE_GRASS: {
        float shade = 0.92f + rand01(xx * 3, yy * 5) * 0.16f;
        r = r * shade;
        g = g * (shade + 0.05f);
        b = b * (shade - 0.02f);
        if ((hash_u32(xx, yy) % 97u) == 0u) g = fminf(g + 25.0f, 255.0f);
      } break;
      case TILE_WATER: {
        float gy = (float)yy / (float)h;
        float wave = 0.04f * sinf((xx + yy * 0.5f) * 0.2f);
        r = 20.0f + 10.0f * gy;
        g = 100.0f + 40.0f * gy;
        b = 180.0f + 50.0f * gy;
        r *= 1.0f + wave;
        g *= 1.0f + wave;
        b *= 1.0f + wave;
      } break;
      case TILE_SAND: {
        float speck = rand01(xx * 7, yy * 11) * 0.25f - 0.125f;
        r *= 1.0f + speck;
        g *= 1.0f + speck * 0.5f;
        b *= 1.0f - speck * 0.3f;
      } break;
      case TILE_ROCK: {
        float rough = rand01(xx * 5, yy * 5) * 0.3f - 0.15f;
        r *= 1.0f + rough;
        g *= 1.0f + rough;
        b *= 1.0f + rough;
      } break;
      case TILE_ROAD_NS:
      case TILE_ROAD_EW:
      case TILE_ROAD_CROSS: {
        float rough = rand01(xx * 13, yy * 9) * 0.15f - 0.075f;
        r = 90.0f * (1.0f + rough);
        g = 92.0f * (1.0f + rough);
        b = 96.0f * (1.0f + rough);
        if (type != TILE_ROAD_EW && ((xx - x_min) % 20) < 2) {
          r = 230.0f;
          g = 210.0f;
          b = 80.0f;
        }
        if (type != TILE_ROAD_NS && (yy % 20) < 2) {
          r = 230.0f;
          g = 210.0f;
          b = 80.0f;
        }
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

      // лёгкое затемнение по краям ромба
      float edge = fminf(fabsf(xx - half_w) / fmaxf(extent, 1.0f), t);
      float shade = 1.0f - 0.15f * fminf(edge * 1.5f, 1.0f);
      r *= shade;
      g *= shade;
      b *= shade;

      // запись пикселя
      uint8_t R = (uint8_t)fminf(fmaxf(r, 0.0f), 255.0f);
      uint8_t G = (uint8_t)fminf(fmaxf(g, 0.0f), 255.0f);
      uint8_t B = (uint8_t)fminf(fmaxf(b, 0.0f), 255.0f);
      uint8_t A = (uint8_t)fminf(fmaxf(a, 0.0f), 255.0f);
      row[xx] = SDL_MapRGBA(surface->format, R, G, B, A);
    }
  }

  SDL_UnlockSurface(surface);
  return surface;
}

Map *map_create(int width, int height, int tile_size) {
  (void)tile_size;
  Map *map = (Map *)calloc(1, sizeof(Map));
  if (!map) { return NULL; }

  map->width = width;
  map->height = height;
  map->tile_width = ISO_TILE_WIDTH;
  map->tile_height = ISO_TILE_HEIGHT;
  map->is_isometric = true;

  // Учитываем, что изометрическая карта имеет ромбовидную форму
  map->world_size.x = (float)(width * ISO_TILE_WIDTH);
  map->world_size.y = (float)(height * ISO_TILE_HEIGHT);

  map->tiles = (TileType *)calloc(width * height, sizeof(TileType));
  if (!map->tiles) {
    free(map);
    return NULL;
  }

  for (int i = 0; i < width * height; i++) { map->tiles[i] = TILE_GRASS; }

  return map;
}

void map_destroy(Map *map) {
  if (!map) return;

  for (int i = 0; i < TILE_MAX; i++) {
    if (map->tile_textures[i]) { SDL_DestroyTexture(map->tile_textures[i]); }
  }

  if (map->tiles) { free(map->tiles); }

  free(map);
}

void map_set_tile(Map *map, int x, int y, TileType type) {
  if (!map || !map->tiles) return;

  if (map_is_valid_position(map, x, y)) { map->tiles[y * map->width + x] = type; }
}

TileType map_get_tile(const Map *map, int x, int y) {
  if (!map || !map->tiles) return TILE_EMPTY;

  if (map_is_valid_position(map, x, y)) { return map->tiles[y * map->width + x]; }
  return TILE_EMPTY;
}

void map_render(Map *map, SDL_Renderer *renderer, const Camera *camera) {
  if (!map || !renderer || !map->tiles) return;

  for (int y = 0; y < map->height; y++) {
    for (int x = 0; x < map->width; x++) {
      TileType tile = map_get_tile(map, x, y);
      if (tile == TILE_EMPTY) continue;

      SDL_Texture *texture = map->tile_textures[tile];
      if (!texture) continue;

      Vector2 world_pos = iso_tile_to_world(x, y);

      Vector2 screen_pos = camera_world_to_screen(camera, world_pos);

      if (screen_pos.x < -100 || screen_pos.x > camera->size.x + 100 || screen_pos.y < -100 ||
          screen_pos.y > camera->size.y + 100) {
        continue;
      }

      SDL_Rect src_rect = {0, 0, ISO_TILE_WIDTH, ISO_TILE_HEIGHT};
      SDL_Rect dst_rect = {(int)screen_pos.x - ISO_TILE_WIDTH / 2,
          (int)screen_pos.y - ISO_TILE_HEIGHT / 2,
          ISO_TILE_WIDTH,
          ISO_TILE_HEIGHT};

      SDL_RenderCopy(renderer, texture, &src_rect, &dst_rect);
    }
  }
}

Vector2 map_world_to_tile(const Map *map, Vector2 world_pos) {
  (void)map;
  return iso_world_to_tile(world_pos);
}

Vector2 map_tile_to_world(const Map *map, Vector2 tile_pos) {
  (void)map;
  return iso_tile_to_world((int)tile_pos.x, (int)tile_pos.y);
}

bool map_is_valid_position(const Map *map, int x, int y) {
  if (!map) return false;
  return (x >= 0 && x < map->width && y >= 0 && y < map->height);
}

void map_create_tile_textures(Map *map, SDL_Renderer *renderer) {
  if (!map || !renderer) return;

  for (int i = 0; i < TILE_MAX; i++) {
    Color color = (Color){128, 128, 128, 255};
    switch (i) {
    case TILE_GROUND: color = (Color){139, 90, 60, 255}; break;
    case TILE_WALL: color = (Color){110, 110, 120, 255}; break;
    case TILE_SHADOW: color = (Color){0, 0, 0, 120}; break;
    case TILE_ROAD_NS:
    case TILE_ROAD_EW:
    case TILE_ROAD_CROSS: color = (Color){95, 95, 100, 255}; break;
    case TILE_WATER: color = (Color){30, 120, 200, 255}; break;
    case TILE_SAND: color = (Color){235, 210, 160, 255}; break;
    case TILE_ROCK: color = (Color){140, 140, 150, 255}; break;
    case TILE_GRASS: color = (Color){90, 155, 85, 255}; break;
    case TILE_BUILDING_RESIDENTIAL: color = (Color){210, 210, 255, 255}; break;
    case TILE_BUILDING_COMMERCIAL: color = (Color){200, 245, 210, 255}; break;
    case TILE_BUILDING_INDUSTRIAL: color = (Color){170, 165, 210, 255}; break;
    case TILE_BUILDING_PUBLIC: color = (Color){255, 255, 220, 255}; break;
    default: break;
    }

    SDL_Surface *surface = create_isometric_tile_surface(color, (TileType)i);
    if (surface) {
      SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surface);
      if (tex) {
        SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
        map->tile_textures[i] = tex;
      }
      SDL_FreeSurface(surface);
    }
  }
}
