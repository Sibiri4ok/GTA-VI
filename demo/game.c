#include "game.h"
#include "dyn_objs.h"
#include "static_objs.h"
#include <engine/coordinates.h>
#include <engine/engine.h>
#include <engine/input.h>
#include <engine/map.h>
#include <stb_ds.h>
#include <stdlib.h>

#define MAP_WIDTH 25
#define MAP_HEIGHT 25
#define STATIC_OBJ_COUNT 200

void game_free(Game *game);
static UIElement hp_bar(Game *game);

Game *game_create() {
  Game *game = calloc(1, sizeof(Game));
  if (!game) { return NULL; }

  Engine *engine = engine_create(800, 600, "GTA VI");
  game->engine = engine;
  if (!engine) {
    game_free(game);
    return NULL;
  }

  TilesInfo ti = {0};
  ti.tile_sprites = calloc(1, sizeof(Sprite));
  ti.tile_sprites[0] = load_sprite("assets/grass_high.png", 1.0f / 7.2f);
  ti.sprite_count = 1;
  ti.tiles = calloc(MAP_WIDTH * MAP_HEIGHT, sizeof(uint32_t));
  ti.sides_height = 64;
  Map *map = map_create(MAP_WIDTH, MAP_HEIGHT, ti);
  if (!map) {
    game_free(game);
    map_free(map);
    return NULL;
  };
  engine_set_map(engine, map);

  DynamicObjects *dyn_objs = create_dynamic_objects(map);
  StaticObjects *st_objs = create_static_objs(map, STATIC_OBJ_COUNT);
  game->st_objs = st_objs;
  game->dyn_objs = dyn_objs;
  if (!dyn_objs || !st_objs) {
    game_free(game);
    map_free(map);
    return NULL;
  }

  game->dyn_objs = dyn_objs;
  game->st_objs = st_objs;
  game->player = dyn_objs_get_player(dyn_objs);
  engine_set_player(engine, game->player);

  game->uis = NULL;
  arrpush(game->uis, hp_bar(game));

  // Create render batch
  game->batch = (RenderBatch){0};

  // Fill batch with game objects
  for (int i = 0; i < arrlen(st_objs->objects); i++) { arrpush(game->batch.objs, &st_objs->objects[i]); }
  GameObject *dyn_objs_arr = dyn_objs_get_objects(dyn_objs);
  for (int i = 0; i < arrlen(dyn_objs_arr); i++) { arrpush(game->batch.objs, &dyn_objs_arr[i]); }
  game->batch.obj_count = arrlen(game->batch.objs);

  // Fill batch with UI elements
  for (int i = 0; i < arrlen(game->uis); i++) { arrpush(game->batch.uis, &game->uis[i]); }
  game->batch.ui_count = arrlen(game->batch.uis);

  return game;
}

void game_free(Game *game) {
  if (!game) return;

  if (game->dyn_objs) free_dyn_objects(game->dyn_objs);
  if (game->st_objs) free_static_objs(game->st_objs);
  if (game->uis) {
    for (int i = 0; i < arrlen(game->uis); i++) {
      free_sprite(game->uis[i].sprite);
      free(game->uis[i].sprite);
    }
    arrfree(game->uis);
  }
  if (game->batch.objs) arrfree(game->batch.objs);
  if (game->batch.uis) arrfree(game->batch.uis);
  if (game->engine) engine_free(game->engine);
  free(game);
}

void game_update(Game *game, Input *input) {
  if (!game || !input) return;

  dyn_objs_update(game->dyn_objs, input, ENGINE_LOGIC_STEP);
}

static UIElement hp_bar(Game *game) {
  UIElement bar = {0};
  bar.mode = UI_POS_ATTACHED;
  bar.position.attached.object = game->player;
  bar.position.attached.offset = (Vector){0, 0};
  bar.z_index = 0;
  Sprite *sprite = malloc(sizeof(Sprite));
  *sprite = load_sprite("assets/hp_bar.png", 1.0f);
  bar.sprite = sprite;
  return bar;
}
