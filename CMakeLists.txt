cmake_minimum_required(VERSION 3.9)
project(algoritmi_fgk_compression)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        colors.h
        main.c
        minunit.h
        defines.h
        fgk/huffmantree.c
        fgk/huffmantree.h
        fgk/utilities.h
        fgk/utlities.c
        console.c console.h)

add_executable(algoritmi_fgk_compression ${SOURCE_FILES})
