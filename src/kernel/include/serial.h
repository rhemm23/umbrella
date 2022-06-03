#ifndef SERIAL_H
#define SERIAL_H

#define SERIAL_COM1 0x3F8

#include "type.h"

void serial_init();
char serial_read_char();

void serial_write_char(char c);
void serial_write_string(char *str);

#endif
