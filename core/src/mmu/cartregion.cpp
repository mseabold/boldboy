#include "cartregion.h"

CartRegion::CartRegion(Cartridge *cart) {
    mCart = cart;
}

CartRegion::~CartRegion() {
}

uint8_t CartRegion::readAddr(uint16_t addr) {
    return 0;
}

void CartRegion::writeAddr(uint16_t addr, uint8_t val) {
}
