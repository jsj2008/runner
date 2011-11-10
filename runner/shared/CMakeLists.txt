cmake_minimum_required (VERSION 2.6)

add_library (shared
   runner.c
)

include_directories (.)
include_directories (../../physics)
include_directories (../../engine)

target_link_libraries (shared engine)

