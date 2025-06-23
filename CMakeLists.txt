cmake_minimum_required(VERSION 3.5)
project(file_manager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build" FORCE)
endif()

add_compile_options(-Wall -Wextra)

find_package(Curses REQUIRED)

set(SOURCES
    src/file_manager.cpp
    src/file_preview.cpp
    src/files_list.cpp
    src/ncurses_setup.cpp
    src/sort_functions.cpp
    src/handle_shell.cpp
)

add_executable(file_manager ${SOURCES})

target_include_directories(file_manager PRIVATE include ${CURSES_INCLUDE_DIR})

target_link_libraries(file_manager PRIVATE ${CURSES_LIBRARIES})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(file_manager PRIVATE -g3 -fsanitize=address)
    target_link_options(file_manager PRIVATE -fsanitize=address)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Profile")
    target_compile_options(file_manager PRIVATE -pg -g3)
    target_link_options(file_manager PRIVATE -pg)
endif()
