#include "game.h"
#include "dyn_objs.h"
#include "static_objs.h"
#include <engine/coordinates.h>
#include <engine/engine.h>
#include <engine/input.h>
#include <engine/map.h>
#include <stb_ds.h>
#include <stdlib.h>

#define MAP_WIDTH 50
#define MAP_HEIGHT 50
#define STATIC_OBJ_COUNT 200

void game_free(Game *game);

Game *game_create() {
  Game *game = calloc(1, sizeof(Game));
  if (!game) { return NULL; }

  Engine *engine = engine_create(800, 600, "GTA VI");
  game->engine = engine;
  if (!engine) {
    game_free(game);
    return NULL;
  }

  Map *map = map_create(MAP_WIDTH, MAP_HEIGHT);
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

  game->objects = NULL;
  for (int i = 0; i < arrlen(st_objs->objects); i++) {
    arrpush(game->objects, &st_objs->objects[i]);
  }
  GameObject *dyn_objs_arr = dyn_objs_get_objects(dyn_objs);
  for (int i = 0; i < arrlen(dyn_objs_arr); i++) { arrpush(game->objects, &dyn_objs_arr[i]); }

  return game;
}

void game_free(Game *game) {
  if (!game) return;

  if (game->dyn_objs) free_dyn_objects(game->dyn_objs);
  if (game->st_objs) free_static_objs(game->st_objs);
  if (game->objects) arrfree(game->objects);
  if (game->engine) engine_free(game->engine);
  free(game);
}

void game_update(Game *game, Input *input) {
  if (!game || !input) return;

  dyn_objs_update(game->dyn_objs, input, ENGINE_LOGIC_STEP);
}
