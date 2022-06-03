#ifndef MOUSE_H
#define MOUSE_H

enum {
  MOUSE_CMD_SET_DEFAULTS = 0xF6,
  MOUSE_CMD_ENABLE_STREAM = 0xF4
};

void mouse_init();

#endif
