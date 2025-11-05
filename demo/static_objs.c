#include "static_objs.h"
#include <engine/map.h>
#include <engine/random.h>
#include <engine/types.h>
#include <stb_ds.h>
#include <stdlib.h>
#include <time.h>

typedef enum {
  OBJ_BUSH1 = 0,
  OBJ_BUSH2,
  OBJ_BUSH3,
  OBJ_TREE,
  OBJ_CACTUS,
  OBJ_PALM,
  OBJ_COUNT
} ObjectType;

static Sprite *load_st_sprites() {
  Sprite *obj_sprites = calloc(OBJ_COUNT, sizeof(Sprite));

  obj_sprites[OBJ_BUSH1] = load_sprite("assets/bush1.png", 1.5f);
  obj_sprites[OBJ_BUSH2] = load_sprite("assets/bush2.png", 1.5f);
  obj_sprites[OBJ_BUSH3] = load_sprite("assets/bush3.png", 1.5f);
  obj_sprites[OBJ_TREE] = load_sprite("assets/tree.png", 2.0f);
  obj_sprites[OBJ_CACTUS] = load_sprite("assets/cactus1.png", 1.0f);
  obj_sprites[OBJ_PALM] = load_sprite("assets/palm.png", 2.5f);

  return obj_sprites;
}

static GameObject *gen_st_objs(Map *map, Sprite *sprites, int count) {
  if (!map || !sprites) return NULL;

  GameObject *objects = NULL;

  int margin = 80;

  while (arrlen(objects) < count) {
    // Random object type (determine first to know sprite size)
    ObjectType type = rand_big() % OBJ_COUNT;
    Sprite *sprite = &sprites[type];

    if (!sprite->pixels) continue;

    VectorU32 world = map_gen_random_position(map, margin);
    VectorU32 map_size = map_get_size(map);
    // Check if sprite corners are within map pixel bounds
    if (world.x + sprite->width >= map_size.x || world.y + sprite->height >= map_size.y) {
      continue;
    }

    // Check that the pixel at this position is not transparent (inside diamond)
    uint32_t center_x = (uint32_t)(world.x + sprite->width / 2);
    uint32_t center_y = (uint32_t)(world.y + sprite->height);
    if (center_x < map_size.x && center_y < map_size.y) {
      uint32_t pixel = map_get_pixel(map, center_x, center_y);
      // Check if pixel is not background (has alpha > 0)
      if ((pixel >> 24) == 0) {
        continue; // Outside diamond shape
      }
    }

    GameObject obj = {0};
    obj.position = (Vector){(float)world.x, (float)world.y};
    obj.cur_sprite = sprite;
    obj.data = NULL;
    arrpush(objects, obj);
  }

  return objects;
}

StaticObjects *create_static_objs(Map *map, int count) {
  if (!map) return NULL;

  StaticObjects *st_objs = calloc(1, sizeof(StaticObjects));
  if (!st_objs) return NULL;

  st_objs->sprites = load_st_sprites();
  if (!st_objs->sprites) {
    free(st_objs);
    return NULL;
  }

  st_objs->objects = gen_st_objs(map, st_objs->sprites, count);
  if (!st_objs->objects) {
    for (int i = 0; i < OBJ_COUNT; i++) { free_sprite(&st_objs->sprites[i]); }
    free(st_objs->sprites);
    free(st_objs);
    return NULL;
  }

  return st_objs;
}

void free_static_objs(StaticObjects *st_objs) {
  if (!st_objs) return;

  if (st_objs->sprites) {
    for (int i = 0; i < OBJ_COUNT; i++) { free_sprite(&st_objs->sprites[i]); }
    free(st_objs->sprites);
  }

  if (st_objs->objects) { arrfree(st_objs->objects); }

  free(st_objs);
}
