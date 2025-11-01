#include "core/engine.h"

int main(void) {
  Engine *engine = engine_create(800, 600, "GTA VI");
  if (!engine) return 1;

  while (engine_begin_frame(engine)) {
    engine_render(engine);
    engine_end_frame(engine);
  }

  engine_destroy(engine);
  return 0;
}
