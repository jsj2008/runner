cmake_minimum_required (VERSION 2.6)

include_directories (.)
include_directories (../engine)

add_executable (converter
   converter.c
   converters/dds.c
   converters/pvr.c
   converters/shader.c
)

add_executable (texture_dump
   texture_dump.c
)

add_executable (world_dump
   world_dump.c
)

#add_library (physics
#   dummy.c
#)

#add_subdirectory ("../engine" ${PROJECT_BINARY_DIR}/tmp/engine)

target_link_libraries (converter engine)
target_link_libraries (texture_dump engine)
target_link_libraries (world_dump engine)

install (TARGETS converter texture_dump world_dump DESTINATION bin)

