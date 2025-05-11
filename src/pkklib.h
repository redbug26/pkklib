#ifndef PKK_LIBRARY_H
#define PKK_LIBRARY_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define u32 uint32_t
#define u16 uint16_t
#define u8 uint8_t

#define s16 int16_t
#define s32 int32_t
#define s8 int8_t

#define TRUE 1
#define FALSE 0

#define RGB565(R, G, B) \
    __builtin_bswap16(  \
        (uint16_t)(((R) & 0xF8) << 8 | ((G) & 0xFC) << 3 | ((B) & 0xF8) >> 3))

#define WHITE RGB565(255, 255, 255)   // 0b1111
#define YELLOW RGB565(255, 255, 0)    // 0b1110
#define LILAC RGB565(255, 128, 255)   // 0b1101
#define BROWN RGB565(255, 128, 0)     // 0b1100
#define FUCHSIA RGB565(255, 64, 255)  // 0b1011
#define RUST RGB565(255, 64, 0)       // 0b1010
#define MAGENTA RGB565(255, 0, 255)   // 0b1001
#define RED RGB565(255, 0, 0)         // 0b1000
#define CYAN RGB565(0, 255, 255)      // 0b0111
#define GREEN RGB565(0, 255, 0)       // 0b0110
#define CERULEAN RGB565(0, 128, 255)  // 0b0101
#define MIDGREEN RGB565(0, 128, 0)    // 0b0100
#define COBALT RGB565(0, 64, 255)     // 0b0011
#define MYRTLE RGB565(0, 64, 0)       // 0b0010
#define BLUE RGB565(0, 0, 255)        // 0b0001
#define BLACK RGB565(0, 0, 0)         // 0b0000
#define BROWN RGB565(255, 128, 0)
#define GRAY RGB565(128, 128, 128)
#define LITEGRAY RGB565(210, 210, 210)
#define ORANGE RGB565(0xff, 0xA5, 0)
#define PINK RGB565(0xFF, 0xA0, 0xAB)
#define GOLD RGB565(0xFF, 0xD7, 0x00)
#define SALMON RGB565(0xFA, 0x80, 0x72)
#define BEIGE RGB565(0xF5, 0xF5, 0xDC)

#include "pkklib_display.h"
#include "pkklib_gfx.h"
#include "pkklib_keyboard.h"
#include "pkklib_sound.h"
#include "pkklib_system.h"

#endif /* ifndef PKK_LIBRARY_H */