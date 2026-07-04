#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include "efi.h"
#include <stdint.h>

typedef struct {
    void* framebuffer;
    uint32_t Width;
    uint32_t Height;
    uint32_t Pixels_Per_ScanLine;
    EFI_GRAPHICS_PIXEL_FORMAT PixelFormat;
    void* font;
    uint32_t font_size;
} FramebufferInfo;
#endif