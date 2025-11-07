#include "render.h"
#include "camera.h"
#include "graphics/alpha_blend.h"
#include "world/map_priv.h"
#include <engine/coordinates.h>
#include <engine/types.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// Compare two game objects by their depth (y-coordinate + height)
int compare_objs_by_depth(const void *a, const void *b) {
  const GameObject *obj_a = *(const GameObject **)a;
  const GameObject *obj_b = *(const GameObject **)b;

  // Sort by bottom edge of sprite for proper isometric depth
  float ya = obj_a->position.y + (obj_a->cur_sprite ? obj_a->cur_sprite->height : 0);
  float yb = obj_b->position.y + (obj_b->cur_sprite ? obj_b->cur_sprite->height : 0);

  return (ya > yb) - (ya < yb);
}

// Render shadow for given object onto framebuffer
static void render_shadow(uint32_t *framebuffer, Camera *camera, GameObject *obj) {
  if (!framebuffer || !obj || !obj->cur_sprite) return;

  // top-left corner of the object in screen coordinates
  Vector top_left = camera_world_to_screen(camera, obj->position);
  int sprite_w = obj->cur_sprite->width;
  int sprite_h = obj->cur_sprite->height;
  int32_t cam_w = (uint32_t)camera->size.x;
  int32_t cam_h = (uint32_t)camera->size.y;

  uint32_t shadow_color = 100 << 24;
  float x_shift_scale = 0.4f;
  // do nothing if shadow is out of the screen
  if (!is_rect_intersect((Rect){top_left, sprite_w + sprite_h * (1.0f + x_shift_scale), sprite_h},
          (Rect){(Vector){0.0f, 0.0f}, cam_w, cam_h})) {
    return;
  }

  for (int y = 0; y < sprite_h; y++) {
    float x_shift = (sprite_h - y) * x_shift_scale;
    int32_t shad_y = top_left.y + y;
    if (shad_y < 0 || shad_y >= cam_h) { continue; }

    for (int x = 0; x < sprite_w; x++) {
      int32_t shad_x = top_left.x + x + x_shift;
      if (shad_x < 0 || shad_x >= cam_w) { continue; }

      uint32_t pix = obj->cur_sprite->pixels[y * sprite_w + x];
      if (((pix >> 24) & 0xFF) == 0) continue; // if pixel is transparent (alpha == 0), skip
      uint32_t fb_idx = shad_y * cam_w + shad_x;
      framebuffer[fb_idx] = alpha_blend(shadow_color, framebuffer[fb_idx]);
    }
  }
}

// Render given game object onto framebuffer considering camera position
void render_object(uint32_t *framebuffer, GameObject *object, Camera *camera) {
  if (!framebuffer || !object || !object->cur_sprite) return;
  Sprite *sprite = object->cur_sprite;

  // Render shadow first
  render_shadow(framebuffer, camera, object);

  Vector obj_screen = camera_world_to_screen(camera, object->position);
  // Return if object is completely off-screen
  if (!is_rect_intersect((Rect){obj_screen, sprite->width, sprite->height},
          (Rect){(Vector){0.0f, 0.0f}, camera->size.x, camera->size.y})) {
    return;
  }

  for (int sy = 0; sy < sprite->height; sy++) {
    int screen_y = obj_screen.y + sy;
    if (screen_y < 0 || screen_y >= camera->size.y) { continue; }

    for (int sx = 0; sx < sprite->width; sx++) {
      int screen_x = obj_screen.x + sx;
      if (screen_x < 0 || screen_x >= camera->size.x) { continue; }

      uint32_t src = sprite->pixels[sy * sprite->width + sx];
      int fb_idx = screen_y * camera->size.x + screen_x;
      uint32_t dst = framebuffer[fb_idx];
      framebuffer[fb_idx] = alpha_blend(src, dst);
    }
  }
}

// Renders multiple game objects
// Objects must be sorted by depth! (y-coordinate and height)
void render_objects(uint32_t *framebuffer, GameObject **objects, int count, Camera *camera) {
  if (!framebuffer || !objects || !camera) return;

  for (int i = 0; i < count; i++) { render_object(framebuffer, objects[i], camera); }
}

// Load prerendered part of the map into framebuffer based on camera position
void load_prerendered(uint32_t *framebuffer, Map *map, Camera *camera) {
  if (!map || !framebuffer || !camera) return;

  Vector top_left_world = camera_screen_to_world(camera, (Vector){0, 0});
  int map_start_x = (int)top_left_world.x;
  int map_start_y = (int)top_left_world.y;

  // Render visible part of the map
  for (int screen_y = 0; screen_y < camera->size.y; screen_y++) {
    int map_y = map_start_y + screen_y;
    if (map_y < 0 || map_y >= map->height_pix) { continue; }

    for (int screen_x = 0; screen_x < camera->size.x; screen_x++) {
      int map_x = map_start_x + screen_x;
      if (map_x < 0 || map_x >= map->width_pix) { continue; }

      int idx = screen_y * (int)camera->size.x + screen_x;
      uint32_t pixel = map->pixels[map_y * map->width_pix + map_x];
      framebuffer[idx] = alpha_blend(pixel, framebuffer[idx]);
    }
  }
}
