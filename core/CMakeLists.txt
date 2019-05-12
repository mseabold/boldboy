set (SRCS
    src/cpu/cpu.cpp
    src/cpu/reg8.cpp
    src/cpu/reg16.cpp
    src/mmu/mmu.cpp
    src/mmu/cartregion.cpp
    src/mmu/ramregion.cpp
    src/mmu/ioregion.cpp
    src/mmu/upperregion.cpp
    src/cart/cartridge.cpp)
add_library(core ${SRCS})

