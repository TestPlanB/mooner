# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

project("mooner_core")

add_library(
        mooner_core

        SHARED

        prevent_pthread_crash.c
        jni_init.c
        mooner_exception.c
        msponge.c
        check_is_destroy_mutex.cpp
        concurrent_hash_map_gap.c
        pthread_mutex_use_after_destroy.c
)
find_package(bytehook REQUIRED CONFIG)

target_link_libraries(mooner_core bytehook::bytehook)
find_package(shadowhook REQUIRED CONFIG)


find_package(xunwind REQUIRED CONFIG)
target_link_libraries(mooner_core xunwind::xunwind)

target_link_libraries(mooner_core shadowhook::shadowhook)

find_library(
        log-lib

        log)


target_link_libraries(
        mooner_core

        ${log-lib})