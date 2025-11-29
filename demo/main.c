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
  if (!game) {
    fprintf(stderr, "Failed to create game\n");
    return 1;
  }
  Engine *engine = game->engine;
  while (engine_begin_frame(engine, update, game)) {
    char fps[100];
    char coords[100];
    snprintf(fps, sizeof(fps), "FPS: %d", (int)engine_get_fps(engine));
    snprintf(coords,
        sizeof(coords),
        "Coords: (%d, %d)",
        (int)game->player->position.x,
        (int)game->player->position.y);
    free_sprite(game->uis[1].sprite);
    free_sprite(game->uis[2].sprite);
    *game->uis[1].sprite = text_sprite(fps, game->fonts[0], (SDL_Color){255, 255, 255, 255});
    *game->uis[2].sprite = text_sprite(coords, game->fonts[0], (SDL_Color){255, 255, 255, 255});

    engine_render(engine, &game->batch);
    engine_end_frame(engine);
  }

  game_free(game);
  return 0;
}

static void update(Input *input, void *user_data) {
  Game *game = (Game *)user_data;

  game_update(game, input);
}
