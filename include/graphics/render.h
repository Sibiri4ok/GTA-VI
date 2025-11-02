#ifndef RENDERER_H
#define RENDERER_H

#include "core/types.h"
#include <game/map.h>
#include <graphics/camera.h>
#include <stdbool.h>
#include <stdint.h>

void render_object(uint32_t *framebuffer, Camera *camera, GameObject *object);

void render_frame_static(Map *map, uint32_t *framebuffer, Camera *camera);

#endif
