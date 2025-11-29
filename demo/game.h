#ifndef GAME_H
#define GAME_H

#include "dyn_objs.h"
#include "static_objs.h"
#include <engine/engine.h>
#include <engine/types.h>

typedef struct Game {
  DynamicObjects *dyn_objs;
  StaticObjects *st_objs;
  UIElement *uis;
  RenderBatch batch; // All objects and UI elements to render
  GameObject *player;
  TTF_Font **fonts;

  Map *map;
  Engine *engine;
} Game;

Game *game_create();
void game_free(Game *game);
void game_update(Game *game, Input *input);

#endif
