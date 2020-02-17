#include "upperregion.h"

#define UPPER_RAM_START 0xFF80
#define OAM_START 0xFE00
#define IO_START 0xFF00
#define ECHOED_RAM_OFFSET 0x2000
#define TO_ECHOED_ADDR_SPACE(_addr) (_addr - ECHOED_RAM_OFFSET)


UpperRegion::UpperRegion(RamRegion *iRam, MemRegion *io, Ppu* ppu) {
    mEchoedRam = iRam;
    mIO = io;
    mPpu = ppu;

    mUpperRam = new RamRegion(UPPER_RAM_START, 0x80);
}

UpperRegion::~UpperRegion() {
    delete mUpperRam;
}

uint8_t UpperRegion::readAddr(uint16_t addr) {
    uint8_t result;

    /* Let the IO controller (via the interrupt controller) handle
     * IER. */
    if(addr == 0xFFFF)
        return mIO->readAddr(addr);

    /* Examine the top bits that determine segments. */
    switch(addr & 0xFF00) {
        /* OAM: 0xFE00-0xFE9F */
        /* UNUSED: 0xFEA0-0xFEFF */
        case OAM_START:
            /* Attempt to read this value from OAM. If it is
             * beyond the bounds of the OAM segment, the region
             * will reject it. */
            result = mPpu->readAddr(addr);
            break;
        /* I/O: 0xFF00 - 0xFF4C */
        /* UNUSED: 0xFF4D - 0xFF79 */
        /* Upper RAM: 0xFF80 - 0xFFFE */
        case 0xFF00:
            switch(addr & 0xFF80) {
                case IO_START:
                    /* TODO: Will IoController handle reads into the unused
                     * FF4C region, or should we kick that here? */
                    result = mIO->readAddr(addr);
                    break;
                case UPPER_RAM_START:
                    result = mUpperRam->readAddr(addr);
                    break;
            }
            break;
        default:
            /* Everything under 0xFE00 in this segment is an echo of internal RAM */
            result = mEchoedRam->readAddr(TO_ECHOED_ADDR_SPACE(addr));
            break;
    }

    return result;
}

void UpperRegion::writeAddr(uint16_t addr, uint8_t val) {

    /* Let the IO controller (via the interrupt controller) handle
     * IER. */
    if(addr == 0xFFFF) {
        mIO->writeAddr(addr, val);
        return;
    }

    /* Examine the top bits that determine segments. */
    switch(addr & 0xFF00) {
        /* OAM: 0xFE00-0xFE9F */
        /* UNUSED: 0xFEA0-0xFEFF */
        case 0xFE00:
            /* Attempt to write this value to OAM. If it is
             * beyond the bounds of the OAM segment, the region
             * will reject the write. */
            mPpu->writeAddr(addr, val);
            break;
        /* I/O: 0xFF00 - 0xFF4C */
        /* UNUSED: 0xFF4D - 0xFF79 */
        /* Upper RAM: 0xFF80 - 0xFFFE */
        case 0xFF00:
            switch(addr & 0xFF80) {
                case 0xFF00:
                    /* TODO: Will IoController handle writes into the unused
                     * FF4C region, or should we kick that here? */
                    mIO->writeAddr(addr, val);
                    break;
                case 0xFF80:
                    mUpperRam->writeAddr(addr, val);
                    break;
            }
            break;
        default:
            /* Everything under 0xFE00 in this segment is an echo of internal RAM */
            mEchoedRam->writeAddr(TO_ECHOED_ADDR_SPACE(addr), val);
            break;
    }
}
