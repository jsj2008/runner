cmake_minimum_required (VERSION 2.6)

find_package (GLUT REQUIRED)

add_executable (runner
   main.c
   crash_handler.c
)

include_directories ("../../physics")
include_directories ("../../engine")
include_directories ("../shared")

target_link_libraries (runner shared ${GLUT_glut_LIBRARY})

install (TARGETS runner DESTINATION bin)

