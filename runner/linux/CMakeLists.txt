cmake_minimum_required (VERSION 2.6)

add_executable (runner
   main.c
)

include_directories ("../../physics")
include_directories ("../../engine")
include_directories ("../shared")

target_link_libraries (runner shared glut)

install (TARGETS runner DESTINATION bin)

