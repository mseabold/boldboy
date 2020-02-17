set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(BoldboyQT
    main.cpp
    boldboyqt.cpp
    boldboyqt.h
    boldboyqt.ui
    emulatorworker.h
    emulatorworker.cpp
)

target_link_libraries(BoldboyQT PRIVATE Qt5::Widgets)
target_link_libraries(BoldboyQT PRIVATE gbcore)
