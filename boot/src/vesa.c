#include "vesa.h"

static char vbe2_signature[4] = { 'V', 'B', 'E', '2' };

int vesa_configure(uint16_t result_buffer) {

  uint16_t mode_buffer = result_buffer + sizeof(vbe_controller_info_t);
  vbe_mode_info_t *mode_info = (vbe_mode_info_t*)mode_buffer;

  vbe_controller_info_t *controller_info = (vbe_controller_info_t*)result_buffer;
  uint32_t result;

  // Copy signature
  for (int i = 0; i < 4; i++) {
    controller_info->signature[i] = vbe2_signature[i];
  }

  // Get VESA info
  __asm__ __volatile__(
    "int $0x10      \n\t"
    "mov %%eax, %0  \n\t"
    :
    "=r" (result)
    :
    "a" (0x4F00),
    "D" (result_buffer)
  );
  if (result != VESA_SUCCESS) {
    return 1;
  }

  uint16_t segment = controller_info->video_mode_ptr[1];
  uint16_t offset = controller_info->video_mode_ptr[0];
  uint16_t mode = 0;

  uint16_t max_height = 0;
  uint16_t max_width = 0;
  uint16_t max_mode = 0xFFFF;

  while (1) {
    __asm__ __volatile__(
      "mov %1, %%fs         \n\t"
      "mov %2, %%di         \n\t"
      "mov %%fs:(%%di), %0  \n\t"
      :
      "=r" (mode)
      :
      "r" (segment),
      "r" (offset)
    );
    if (mode == 0xFFFF) {
      break;
    } else {
      __asm__ __volatile__(
        "int $0x10      \n\t"
        "mov %%eax, %0  \n\t"
        :
        "=r" (result)
        :
        "a" (0x4F01),
        "c" (mode),
        "D" (mode_buffer)
      );
      if (result != VESA_SUCCESS) {
        return 1;
      } else if (
        mode_info->bpp == 32 &&
        mode_info->height >= max_height &&
        mode_info->width >= max_width
      ) {
        max_height = mode_info->height;
        max_width = mode_info->width;
        max_mode = mode;
      }
      offset += 2;
    }
  }

  // Assure a mode was found
  if (max_mode == 0xFFFF) {
    return 1;
  }

  // Get info on best mode
  __asm__ __volatile__(
    "int $0x10      \n\t"
    "mov %%eax, %0  \n\t"
    :
    "=r" (result)
    :
    "a" (0x4F01),
    "c" (max_mode),
    "D" (mode_buffer)
  );
  if (result != VESA_SUCCESS) {
    return 1;
  }

  // Set the best mode
  __asm__ __volatile__(
    "int $0x10      \n\t"
    "mov %%eax, %0  \n\t"
    :
    "=r" (result)
    :
    "a" (0x4F02),
    "b" (max_mode)
  );
  if (result != VESA_SUCCESS) {
    return 1;
  }

  return 0;
}
