cmake_minimum_required (VERSION 3.1.0)
project(gbemu)
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

add_subdirectory(core)
add_subdirectory(app/linux_cli)
add_subdirectory(tests)
add_subdirectory(ports)
add_subdirectory(app/qt/BoldboyQT)
