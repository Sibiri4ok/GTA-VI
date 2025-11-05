#ifndef DYN_OBJS_H
#define DYN_OBJS_H

#include <engine/input.h>
#include <engine/map.h>
#include <engine/types.h>

typedef struct DynamicObjects DynamicObjects;

DynamicObjects *create_dynamic_objects(Map *map);
void free_dyn_objects(DynamicObjects *dyn_objs);
// Update dynamic objects (movement, animation, etc.).
// Delta time is logic timestep in seconds.
void dyn_objs_update(DynamicObjects *dyn_objs, Input *input, float delta_time);

GameObject *dyn_objs_get_player(DynamicObjects *dyn_objs);
GameObject *dyn_objs_get_objects(DynamicObjects *dyn_objs);

#endif
