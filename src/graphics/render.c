#include "render.h"
#include "camera.h"
#include "graphics/alpha_blend.h"
#include "world/map_priv.h"
#include <engine/coordinates.h>
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
static void render_shadow(uint32_t *framebuffer, Camera *camera, GameObject *object) {
  if (!framebuffer || !object || !object->cur_sprite) return;

  // Shadow size - simple ellipse
  int shadow_width = object->cur_sprite->width / 2;
  int shadow_height = shadow_width / 3;

  // Sun from bottom-left to top-right
  int shadow_offset_x = object->cur_sprite->width / 4;
  int shadow_offset_y = -object->cur_sprite->height / 6;

  // Shadow center in screen coordinates
  Vector obj_screen = camera_world_to_screen(camera, object->position);
  int center_x = obj_screen.x + object->cur_sprite->width / 2 + shadow_offset_x;
  int center_y = obj_screen.y + object->cur_sprite->height + shadow_offset_y;

  // Return if shadow is completely off-screen
  int shadow_left = center_x - shadow_width;
  int shadow_right = center_x + shadow_width;
  int shadow_top = center_y - shadow_height;
  int shadow_bottom = center_y + shadow_height;

  if (shadow_right < 0 || shadow_left >= camera->size.x || shadow_bottom < 0 ||
      shadow_top >= camera->size.y) {
    return;
  }

  for (int y = -shadow_height; y <= shadow_height; y++) {
    for (int x = -shadow_width; x <= shadow_width; x++) {
      // Ellipse equation: (x/a)^2 + (y/b)^2 <= 1
      float nx = (float)x / (float)shadow_width;
      float ny = (float)y / (float)shadow_height;
      float dist = nx * nx + ny * ny;

      if (dist <= 1.0f) {
        // Gradient: darker in center, lighter at edges
        float alpha_factor = 1.0f - dist;
        alpha_factor = powf(alpha_factor, 1.5f); // Smooth and saturated falloff
        uint8_t alpha = (uint8_t)(alpha_factor * 140.0f);

        uint32_t shadow_color = (alpha << 24); // Black with varying alpha

        int screen_x = center_x + x;
        int screen_y = center_y + y;

        // Check visibility
        if (camera_is_visible(camera, (Vector){screen_x, screen_y})) {
          int fb_idx = screen_y * (int)camera->size.x + screen_x;
          framebuffer[fb_idx] = alpha_blend(shadow_color, framebuffer[fb_idx]);
        }
      }
    }
  }
}

// Render given game object onto framebuffer considering camera position
void render_object(uint32_t *framebuffer, GameObject *object, Camera *camera) {
  if (!framebuffer || !object || !object->cur_sprite) return;
  Sprite *sprite = object->cur_sprite;

  // Return if object is completely off-screen
  Vector obj_screen = camera_world_to_screen(camera, object->position);
  if (obj_screen.x + sprite->width < 0 || obj_screen.x >= camera->size.x ||
      obj_screen.y + sprite->height < 0 || obj_screen.y >= camera->size.y) {
    return;
  }

  // Render shadow first
  render_shadow(framebuffer, camera, object);

  for (int sy = 0; sy < sprite->height; sy++) {
    for (int sx = 0; sx < sprite->width; sx++) {
      int src_x = object->flip_horizontal ? (sprite->width - 1 - sx) : sx;

      int screen_x = obj_screen.x + sx;
      int screen_y = obj_screen.y + sy;

      if (!camera_is_visible(camera, (Vector){screen_x, screen_y})) { continue; }

      uint32_t src = sprite->pixels[sy * sprite->width + src_x];
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

// Load prerendered map portion into framebuffer based on camera position
// TODO: optimize camera bounds checking by rectangle intersection, not every pixel
void load_prerendered(uint32_t *framebuffer, Map *map, Camera *camera) {
  if (!map || !framebuffer || !camera) return;

  Vector top_left_world = camera_screen_to_world(camera, (Vector){0, 0});
  int map_start_x = (int)top_left_world.x;
  int map_start_y = (int)top_left_world.y;

  // Render visible part of the map
  for (int screen_y = 0; screen_y < camera->size.y; screen_y++) {
    for (int screen_x = 0; screen_x < camera->size.x; screen_x++) {
      int map_x = map_start_x + screen_x;
      int map_y = map_start_y + screen_y;

      // Skip pixels outside map
      if (map_x < 0 || map_x >= map->width_pix || map_y < 0 || map_y >= map->height_pix) {
        continue;
      }
      int idx = screen_y * (int)camera->size.x + screen_x;
      uint32_t pixel = map->pixels[map_y * map->width_pix + map_x];
      framebuffer[idx] = alpha_blend(pixel, framebuffer[idx]);
    }
  }
}
