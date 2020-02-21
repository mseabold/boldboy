#ifndef __REGISTERS_H__
#define __REGISTERS_H__

#include <stdint.h>
#include "io_regs.h"
#include "logger.h"

#define OFFSET_9000 0x1000
#define OFFSET_9800 0x1800
#define OFFSET_9C00 0x1C00

class PpuRegisters {
    public:
        PpuRegisters() {
            LCDC = 0;
            SCY = 0;
            SCX = 0;
            LY = 0;
            LYC = 0;
            BGP = 0;
            OBP0 = 0;
            OBP1 = 0;
            WY = 0;
            WX = 0;
            STAT = 0;

            bgMapOffset = OFFSET_9800;
            winMapOffset = OFFSET_9800;
            tileDataOffset = OFFSET_9000;
        }

        void write(uint16_t addr, uint8_t val) {
            switch(addr) {
                case IOREG_LCDC:
                    bgMapOffset = ((val & IOREG_LCDC_BG_TILE_MAP_SEL_MASK) == IOREG_LCDC_BG_TILE_MAP_SEL_9800)?OFFSET_9800:OFFSET_9C00;
                    tileDataOffset = ((val & IOREG_LCDC_TILE_DATA_SEL_MASK) == IOREG_LCDC_TILE_DATA_SEL_8000)?0:OFFSET_9000;
                    winMapOffset = ((val & IOREG_LCDC_WIN_TILE_MAP_SEL_MASK) == IOREG_LCDC_WIN_TILE_MAP_SEL_9800)?OFFSET_9800:OFFSET_9C00;
                    LCDC = val;
                    DLOG("LCDC write: 0x%02x\n", LCDC);
                    break;
                case IOREG_STAT:
                    STAT = (val & IOREG_STAT_INTRS_MASK) | (STAT & ~IOREG_STAT_INTRS_MASK);
                    break;
                case IOREG_SCY:
                    SCY = val;
                    break;
                case IOREG_SCX:
                    SCX = val;
                    break;
                case IOREG_LYC:
                    LYC = val;
                    break;
                case IOREG_BGP:
                    BGP = val;
                    break;
                case IOREG_OBP0:
                    OBP0 = val;
                    break;
                case IOREG_OBP1:
                    OBP1 = val;
                    break;
                case IOREG_WY:
                    WY = val;
                    break;
                case IOREG_WX:
                    WX = val;
                    break;
                }
        }

        uint8_t read(uint16_t addr) {
            switch(addr) {
                case IOREG_LCDC:
                    return LCDC;
                case IOREG_STAT:
                    return STAT;
                case IOREG_SCY:
                    return SCY;
                case IOREG_SCX:
                    return SCX;
                case IOREG_LY:
                    return LY;
                case IOREG_LYC:
                    return LYC;
                case IOREG_BGP:
                    return BGP;
                case IOREG_OBP0:
                    return OBP0;
                case IOREG_OBP1:
                    return OBP1;
                case IOREG_WY:
                    return WY;
                case IOREG_WX:
                    return WX;
            }

            return 0xff;
        }

        uint8_t LCDC;
        uint8_t SCY;
        uint8_t SCX;
        uint8_t LY;
        uint8_t LYC;
        uint8_t BGP;
        uint8_t OBP0;
        uint8_t OBP1;
        uint8_t WY;
        uint8_t WX;
        uint8_t STAT;

        uint16_t bgMapOffset;
        uint16_t winMapOffset;
        uint16_t tileDataOffset;
};

#endif /* __REGISTERS_H__ */
