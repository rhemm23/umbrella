#ifndef PIC_H
#define PIC_H

#define PIC_MASTER_COMMAND 0x20
#define PIC_MASTER_DATA 0x21

#define PIC_SLAVE_COMMAND 0xA0
#define PIC_SLAVE_DATA 0xA1

#define PIC_INIT 0x10
#define PIC_8086 0x01
#define PIC_EOI 0x20

#include "type.h"

void pic_remap(uint8_t master_offset, uint8_t slave_offset);

#endif
