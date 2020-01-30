add_library(serialconsole src/serial_console.cpp)
target_include_directories(serialconsole PUBLIC inc)
target_link_libraries(serialconsole PUBLIC gbcore)
