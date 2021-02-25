find_package (Python3 COMPONENTS Interpreter REQUIRED)

set (TOOLSDIR ${PROJECT_SOURCE_DIR}/tools)

add_custom_command(
    OUTPUT ${PROJECT_BINARY_DIR}/core/src/cpu/optable.cpp
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/core/src/cpu
    COMMAND ${Python3_EXECUTABLE} ${TOOLSDIR}/gen_optable.py ${TOOLSDIR}/handlers.json ${TOOLSDIR}/opcodes.json -f ${PROJECT_BINARY_DIR}/core/src/cpu/optable.cpp
    DEPENDS ${TOOLSDIR}/gen_optable.py ${TOOLSDIR}/handlers.json ${TOOLSDIR}/opcodes.json
)
set (SRCS
    src/boldboy.cpp
    src/cpu/cpu.cpp
    src/cpu/cpu_opcodes.cpp
    src/cpu/reg8.cpp
    src/cpu/reg16.cpp
    src/cpu/int_controller.cpp
    src/mmu/mmu.cpp
    src/mmu/ramregion.cpp
    src/mmu/upperregion.cpp
    src/cart/cartridge.cpp
    src/cart/cart_rom.cpp
    src/cart/cart_mbc1.cpp
    src/cart/cart_mbc2.cpp
    src/cart/cart_mbc3.cpp
    src/cart/cart_mbc5.cpp
    src/io/io_controller.cpp
    src/io/io_serial.cpp
    src/io/io_timer.cpp
    src/io/io_oam_dma.cpp
    src/io/io_joypad.cpp
    src/log/logger.cpp
    src/log/console.cpp
    src/ppu/ppu.cpp
    src/ppu/fetcher.cpp
    src/ppu/tileline.cpp
    src/ppu/oamentry.cpp
    src/ppu/pixelfifo.cpp)

add_library(gbcore ${SRCS} ${PROJECT_BINARY_DIR}/core/src/cpu/optable.cpp)

target_include_directories(gbcore
    PUBLIC
        inc
)

if(ENABLE_TEST_HARNESS)
    target_compile_definitions(gbcore PUBLIC ENABLE_TEST_HARNESS)
endif()
