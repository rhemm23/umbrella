#include "print.h"

void print_char(char c) {
  __asm__ __volatile__(
    "int $0x10"
    : :
    "a" (0x0E00 | c)
  );
}

void print(char *str) {
  while (*str) {
    print_char(*str);
    str++;
  }
}

void print_newline() {
  print_char('\r');
  print_char('\n');
}

void printx(uint64_t value) {
  print_char('0');
  print_char('x');
  if (!value) {
    print_char('0');
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
      print_char(buffer[i]);
    }
  }
}
