#include "static_objs.h"
#include <engine/map.h>
#include <engine/random.h>
#include <engine/types.h>
#include <math.h>
#include <stb_ds.h>
#include <stdlib.h>
#include <time.h>

#define hypotenuse(a, b) (sqrtf((a) * (a) + (b) * (b)))

typedef enum { OBJ_BUSH1 = 0, OBJ_BUSH2, OBJ_BUSH3, OBJ_TREE, OBJ_CACTUS, OBJ_PALM, OBJ_COUNT } ObjectType;

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

  while (arrlen(objects) < count) {
    // Random object type (determine first to know sprite size)
    ObjectType type = rand_big() % OBJ_COUNT;
    Sprite *sprite = &sprites[type];

    if (!sprite->pixels) continue;

    int margin = hypotenuse((float)sprite->width, (float)sprite->height);
    VectorU32 world = map_gen_random_position(map, margin);

    GameObject obj = {0};
    obj.position = (Vector){(float)world.x, (float)world.y - sprite->height / 2};
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
