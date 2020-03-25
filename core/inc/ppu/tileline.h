#ifndef __TILELINE_H__
#define __TILELINE_H__

#include <stdint.h>
#include "ppu/pixel.h"

class TileLine {
    public:
        TileLine();
        TileLine(uint8_t *tileData, Pixel::PixelType pixelType, uint8_t flags);
        TileLine(const TileLine &other);

        void load(uint8_t *tileData, Pixel::PixelType pixelType, uint8_t flags);
        void load(uint8_t *tileData, Pixel::PixelType pixelType, uint8_t flags, bool flipped);
        void setFlipped(bool flipped);
        bool isFlipped();
        Pixel get(uint8_t idx);

        static TileLine empty();
    private:
        bool mIsFlipped;
        Pixel mPixels[8];
};

#endif /* __TILELINE_H__ */
