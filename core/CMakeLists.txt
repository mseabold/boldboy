find_package (Python3 COMPONENTS Interpreter REQUIRED)
set (TOOLSDIR ${CMAKE_SOURCE_DIR}/tools)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/cpu/optable.cpp
    COMMAND ${Python3_EXECUTABLE} ${TOOLSDIR}/gen_optable.py ${TOOLSDIR}/handlers.json ${TOOLSDIR}/opcodes.json -f ${CMAKE_CURRENT_BINARY_DIR}/src/cpu/optable.cpp
    DEPENDS ${TOOLSDIR}/gen_optable.py ${TOOLSDIR}/handlers.json ${TOOLSDIR}/opcodes.json
)
set (SRCS
    src/cpu/cpu.cpp
    src/cpu/cpu_opcodes.cpp
    src/cpu/reg8.cpp
    src/cpu/reg16.cpp
    src/mmu/mmu.cpp
    src/mmu/cartregion.cpp
    src/mmu/ramregion.cpp
    src/mmu/ioregion.cpp
    src/mmu/upperregion.cpp
    src/cart/cartridge.cpp)
add_library(core ${SRCS} ${CMAKE_CURRENT_BINARY_DIR}/src/cpu/optable.cpp)

