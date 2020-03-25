#ifndef __OAMENTRY_H__
#define __OAMENTRY_H__

#include <stdint.h>

#define OAM_ENTRY_FLAG_OBJ_BG_PRIO_MASK 0x80
#define OAM_ENTRY_FLAG_OBJ_BG_PRIO_ABOVE 0x00
#define OAM_ENTRY_FLAG_OBJ_BG_PRIO_BELOW 0x80
#define OAM_ENTRY_FLAG_Y_FLIP 0x40
#define OAM_ENTRY_FLAG_X_FLIP 0x20
#define OAM_ENTRY_FLAG_DMG_PALLETTE 0x10

// The following flags are only for GBC, but I'll go ahead and define them
#define OAM_ENTRY_FLAG_VRAM_BANK_MASK 0x08
#define OAM_ENTRY_FLAG_VRAM_BANK_0 0x00
#define OAM_ENTRY_FLAG_VRAM_BANK_1 0x08
#define OAM_ENTRY_FLAG_GBC_PALLETTE_MASK 0x07

class OAMEntry {
    public:
        OAMEntry();
        OAMEntry(uint8_t OAMData[4], uint8_t oamIdx);
        OAMEntry(const OAMEntry &other);
        uint8_t oamIdx;
        uint8_t x;
        uint8_t y;
        uint8_t tile;
        uint8_t flags;
        bool yFlip;
        bool xFlip;
        bool belowBG;
        bool OBP1; // Will need to change for GBC
};

#endif /* __OAMENTRY_H__ */
