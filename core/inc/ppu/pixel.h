#ifndef __PIXEL_H__
#define __PIXEL_H__

#include <stdint.h>

#define PIXEL_FLAGS_SPRITE_BELOW_BG 0x01

class Pixel {
    public:
        typedef enum {
            ptBackground,
            ptWindow,
            ptSprite0, // Sprite pixel with OBP0
            ptSprite1  // Sprite pixel with OBP1
        } PixelType;

        Pixel() : Pixel(0, ptBackground, 0) {}

        Pixel(uint8_t value, PixelType type, uint8_t flags) {
            this->value = value;
            this->type = type;
            this->flags = flags;
        }

        Pixel(const Pixel &other) {
            this->value = other.value;
            this->type = other.type;
            this->flags = other.flags;
        }

        uint8_t value;
        uint8_t type;
        uint8_t flags;
};

#endif /* __PIXEL_H__ */
