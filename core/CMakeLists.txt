set (SRCS
    src/cpu/cpu.cpp
    src/cpu/reg8.cpp
    src/cpu/reg16.cpp)
add_library(core ${SRCS})

