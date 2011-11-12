cmake_minimum_required (VERSION 2.6)

add_subdirectory ("./levels" ${CMAKE_CURRENT_BINARY_DIR}/levels)
add_subdirectory ("./shaders" ${CMAKE_CURRENT_BINARY_DIR}/shaders)
add_subdirectory ("./textures" ${CMAKE_CURRENT_BINARY_DIR}/textures)

add_custom_target (assets
   DEPENDS levels shaders textures
)

