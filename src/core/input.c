#include "core/input.h"
#include <string.h>

void input_init(Input *input) {
  if (!input) return;
  memset(input, 0, sizeof(*input));
}

void input_clear(Input *input) {
  if (!input) return;
  input->action_count = 0;
}

void input_push_action(Input *input, InputAction action) {
  if (!input || input->action_count >= INPUT_MAX_ACTIONS) return;
  input->actions[input->action_count++] = action;
  if (action == INPUT_ACTION_ESC) { input->quit = true; }
}

bool input_should_quit(const Input *input) {
  return input ? input->quit : false;
}
