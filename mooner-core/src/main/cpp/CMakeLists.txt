# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

project("mooner_core")

add_library(
        mooner_core

        SHARED

        core.c)
find_package(bytehook REQUIRED CONFIG)

target_link_libraries(mooner_core bytehook::bytehook)


find_library(
        log-lib

        log)


target_link_libraries(
        mooner_core

        ${log-lib})