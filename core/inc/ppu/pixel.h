#ifndef __PIXEL_H__
#define __PIXEL_H__

#include <stdint.h>


class Pixel {
    public:
        typedef enum {
            ptBackground,
            ptWindow,
            ptSprite0, // Sprite pixel with OBP0
            ptSprite1  // Sprite pixel with OBP1
        } PixelType;

        Pixel() : Pixel(0, ptBackground) {}

        Pixel(uint8_t value, PixelType type) {
            this->value = value;
            this->type = type;
        }

        Pixel(const Pixel &other) {
            this->value = other.value;
            this->type = other.type;
        }

        uint8_t value;
        uint8_t type;
};

#endif /* __PIXEL_H__ */
