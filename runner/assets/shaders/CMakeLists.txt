cmake_minimum_required (VERSION 2.6)

set (SHADERS
   bbox
   button
   crate
   level
   physics
   skybox
)

set (PROCESSED_SHADERS)
foreach (_file ${SHADERS})
   set (VERTEX_SHADER "${_file}.vs")
   set (PIXEL_SHADER "${_file}.ps")
   set (PROCESSED_SHADER_PATH "${CMAKE_CURRENT_BINARY_DIR}/${_file}.shader")
   add_custom_command (
      OUTPUT ${PROCESSED_SHADER_PATH}
      COMMAND cp "${CMAKE_CURRENT_SOURCE_DIR}/${VERTEX_SHADER}" "${PROCESSED_SHADER_PATH}" 
      DEPENDS ${VERTEX_SHADER} ${PIXEL_SHADER}
   )
   list (APPEND PROCESSED_SHADERS ${PROCESSED_SHADER_PATH})
endforeach ()

add_custom_target (shaders
   DEPENDS ${PROCESSED_SHADERS}
)

install (FILES ${PROCESSED_SHADERS} DESTINATION data/shaders)

