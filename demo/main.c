#include "static_objs.h"
#include <engine/coordinates.h>
#include <engine/engine.h>
#include <engine/input.h>
#include <engine/map.h>
#include <math.h>
#include <stb_ds.h>

#define MAP_WIDTH 50
#define MAP_HEIGHT 50
#define STATIC_OBJ_COUNT 100
#define PLAYER_SPEED 5.0f

typedef struct mario_sprites {
  Sprite def;
  Sprite back;
  Sprite forward;
} mario_sprites;

static mario_sprites load_mario_sprs();
static GameObject *gen_dyn_objects(mario_sprites *sprs);
static void mario_update(Input *input, GameObject *player);
static void update(Input *input, void *user_data);

typedef struct callback_data {
  GameObject *objects;
} callback_data;

int main(void) {
  Engine *engine = engine_create(800, 600, "GTA VI");
  Map *map = map_create(MAP_WIDTH, MAP_HEIGHT);
  if (!engine || !map) return 1;

  engine_set_map(engine, map);

  mario_sprites sprs = load_mario_sprs();
  GameObject *dyn_objs = gen_dyn_objects(&sprs);
  engine_set_player(engine, &dyn_objs[0]);

  GameObject *st_objs = gen_st_objs(map, STATIC_OBJ_COUNT);

  callback_data cb_data = {dyn_objs};
  GameObject **objects = NULL;
  for (int i = 0; i < arrlen(st_objs); i++) { arrpush(objects, &st_objs[i]); }
  for (int i = 0; i < arrlen(dyn_objs); i++) { arrpush(objects, &dyn_objs[i]); }
  while (engine_begin_frame(engine, update, &cb_data)) {
    engine_render(engine, objects, arrlen(objects));
    engine_end_frame(engine);
  }

  arrfree(dyn_objs);
  arrfree(st_objs);
  arrfree(objects);
  free_sprite(&sprs.def);
  free_sprite(&sprs.back);
  free_sprite(&sprs.forward);
  engine_free(engine);
  return 0;
}

static mario_sprites load_mario_sprs() {
  mario_sprites sprs;
  float scale = 1.0f / 16.0f;
  sprs.def = load_sprite("assets/mario.png", scale);
  sprs.back = load_sprite("assets/mario_back.png", scale);
  sprs.forward = load_sprite("assets/mario_forward.png", scale);
  return sprs;
}

static GameObject *gen_dyn_objects(mario_sprites *sprs) {
  GameObject *objects = NULL;
  GameObject player = (GameObject){.cur_sprite = &sprs->def,
      .flip_horizontal = false,
      .velocity = (Vector){0.0f, 0.0f},
      .position = iso_tile_to_world(MAP_WIDTH / 2, MAP_HEIGHT / 2, MAP_HEIGHT),
      .data = sprs};
  GameObject npc1 = player;
  GameObject npc2 = player;
  npc1.cur_sprite = &sprs->back;
  npc1.position = iso_tile_to_world(MAP_WIDTH / 2 - 3, MAP_HEIGHT / 2 - 3, MAP_HEIGHT);
  npc2.position = iso_tile_to_world(MAP_WIDTH / 2 + 3, MAP_HEIGHT / 2 + 3, MAP_HEIGHT);
  arrpush(objects, player);
  arrpush(objects, npc1);
  arrpush(objects, npc2);
  return objects;
}

static void mario_update(Input *input, GameObject *player) {
  float ax = 0.0f, ay = 0.0f;
  if (input->a) ax -= 1.0f;
  if (input->d) ax += 1.0f;
  if (input->w) ay -= 1.0f;
  if (input->s) ay += 1.0f;

  // Normalize diagonal movement
  float len = sqrtf(ax * ax + ay * ay);
  if (len > 0.0f) {
    ax /= len;
    ay /= len;
  }

  // Update velocity and position
  player->velocity.x = ax * PLAYER_SPEED;
  player->velocity.y = ay * PLAYER_SPEED;
  player->position.x += player->velocity.x;
  player->position.y += player->velocity.y;

  // Update sprite orientation (vertical takes priority over horizontal)
  mario_sprites *sprs = (mario_sprites *)player->data;
  if (player->velocity.y < -0.1f) {
    player->cur_sprite = &sprs->forward;
  } else if (player->velocity.y > 0.1f) {
    player->cur_sprite = &sprs->back;
  } else if (fabs(player->velocity.x) > 0.1f) {
    player->cur_sprite = &sprs->def;
  }

  if (player->velocity.x < -0.1f) {
    player->flip_horizontal = true;
  } else if (player->velocity.x > 0.1f) {
    player->flip_horizontal = false;
  }

  player->velocity = (Vector){0.0f, 0.0f};
}

// Npc push player away when too close
static void npc_push_away(GameObject *player, GameObject *npc) {
  float dx = player->position.x - npc->position.x;
  float dy = player->position.y - npc->position.y;
  float dist = sqrtf(dx * dx + dy * dy);
  if (dist < 100.0f) {
    if (dist < 1.0f) { dist = 1.0f; }
    player->position.x += (dx / dist) * 2.0f;
    player->position.y += (dy / dist) * 2.0f;
  }
}

// Npc runs away from player when too close
static void npc_run_away(GameObject *player, GameObject *npc) {
  float dx = player->position.x - npc->position.x;
  float dy = player->position.y - npc->position.y;
  float dist = sqrtf(dx * dx + dy * dy);
  if (dist < 100.0f) {
    if (dist < 1.0f) { dist = 1.0f; }
    npc->position.x -= (dx / dist) * 3.0f;
    npc->position.y -= (dy / dist) * 3.0f;
  }
}

static void update(Input *input, void *user_data) {
  callback_data *cb_data = (callback_data *)user_data;

  mario_update(input, &cb_data->objects[0]);
  npc_push_away(&cb_data->objects[0], &cb_data->objects[1]);
  npc_run_away(&cb_data->objects[0], &cb_data->objects[2]);
}
