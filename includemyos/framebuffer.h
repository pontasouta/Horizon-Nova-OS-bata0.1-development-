#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include <stdint.h>
typedef struct {
    void* framebuffer;
    uint32_t Width;
    uint32_t Height;
    uint32_t Pixels_Per_ScanLine;
    void* font; uint32_t font_size;
} FramebufferInfo;
#endif