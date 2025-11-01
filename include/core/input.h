#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

#define INPUT_MAX_ACTIONS 128

typedef enum {
  INPUT_ACTION_W,
  INPUT_ACTION_A,
  INPUT_ACTION_S,
  INPUT_ACTION_D,
  INPUT_ACTION_QUIT
} InputAction;

typedef struct {
  InputAction actions[INPUT_MAX_ACTIONS];
  int action_count;
  bool quit;
} Input;

void input_init(Input *input);
void input_clear(Input *input);
void input_push_action(Input *input, InputAction action);
bool input_should_quit(const Input *input);

#endif
