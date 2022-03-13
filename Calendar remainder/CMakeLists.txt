cmake_minimum_required(VERSION 3.21)
project(Qt_lab_1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "~/Qt/6.2.3/gcc_64/lib/cmake")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(Qt_lab_1 main.cpp main_window.cpp main_window.h)
target_link_libraries(Qt_lab_1
        Qt::Core
        Qt::Gui
        Qt::Widgets
        )

