#include "ppu/oamentry.h"

OAMEntry::OAMEntry(uint8_t OAMData[4]) {
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
    y = other.y;
    x = other.x;
    tile = other.tile;
    flags = other.flags;
    yFlip = other.yFlip;
    xFlip = other.xFlip;
    belowBG = other.belowBG;
    OBP1 = other.OBP1;
}
