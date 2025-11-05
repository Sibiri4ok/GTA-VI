#include "game.h"
#include "static_objs.h"
#include "stb_ds.h"
#include <engine/coordinates.h>
#include <engine/engine.h>
#include <engine/input.h>
#include <engine/map.h>
#include <math.h>
#include <stb_ds.h>
#include <stdio.h>

static void update(Input *input, void *user_data);

int main(void) {

  Game *game = game_create();
  if (!game) { return 1; }
  Engine *engine = game->engine;
  while (engine_begin_frame(engine, update, game)) {
    engine_render(engine, game->objects, arrlen(game->objects));
    printf("FPS: %d\n", (int)engine_get_fps(engine));
    engine_end_frame(engine);
  }

  game_free(game);
  return 0;
}

static void update(Input *input, void *user_data) {
  Game *game = (Game *)user_data;

  game_update(game, input);
}
