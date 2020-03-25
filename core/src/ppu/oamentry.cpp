#include "ppu/oamentry.h"

OAMEntry::OAMEntry() {
    oamIdx = 0;
    y = 0;
    x = 0;
    tile = 0;
    flags = 0;
    xFlip = false;
    yFlip = false;
    belowBG = false;
    OBP1 = false;
}

OAMEntry::OAMEntry(uint8_t OAMData[4], uint8_t oamIdx) {
    this->oamIdx = oamIdx;
    y = OAMData[0];
    x = OAMData[1];
    tile = OAMData[2];
    flags = OAMData[3];

    yFlip = ((flags & OAM_ENTRY_FLAG_Y_FLIP) != 0);
    xFlip = ((flags & OAM_ENTRY_FLAG_X_FLIP) != 0);
    belowBG = ((flags & OAM_ENTRY_FLAG_OBJ_BG_PRIO_MASK) == OAM_ENTRY_FLAG_OBJ_BG_PRIO_BELOW);
    OBP1 = ((flags & OAM_ENTRY_FLAG_DMG_PALLETTE) != 0);
}

OAMEntry::OAMEntry(const OAMEntry &other) {
    oamIdx = other.oamIdx;
    y = other.y;
    x = other.x;
    tile = other.tile;
    flags = other.flags;
    yFlip = other.yFlip;
    xFlip = other.xFlip;
    belowBG = other.belowBG;
    OBP1 = other.OBP1;
}
