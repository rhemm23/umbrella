#include "serial.h"
#include "log.h"

void log_init() {
  serial_write_string("\n\nBEGIN LOG\n---------\n");
}

void logx(uint64_t value) {
  serial_write_char('0');
  serial_write_char('x');
  if (!value) {
    serial_write_char('0');
  } else {
    char buffer[25];
    int cnt = 0;
    while (value) {
      char digit = (char)(value % 16);
      if (digit >= 0xA) {
        digit = 'A' + (digit - 0xA);
      } else {
        digit += '0';
      }
      buffer[cnt++] = digit;
      value /= 16;
    }
    for (int i = cnt - 1; i >= 0; i--) {
      serial_write_char(buffer[i]);
    }
  }
}

void log_info(char *str) {
  serial_write_string(ANSI_COLOR_BLUE "INFO: " ANSI_COLOR_RESET);
  serial_write_string(str);
  serial_write_char('\n');
}

void log_warn(char *str) {
  serial_write_string(ANSI_COLOR_YELLOW "WARN: " ANSI_COLOR_RESET);
  serial_write_string(str);
  serial_write_char('\n');
}

void log_error(char *str) {
  serial_write_string(ANSI_COLOR_RED "ERROR: " ANSI_COLOR_RESET);
  serial_write_string(str);
  serial_write_char('\n');
}
