add_executable(linux_cli main.cpp ${CMAKE_SOURCE_DIR}/ports/common/src/serial_console.cpp)
target_link_libraries(linux_cli core)
