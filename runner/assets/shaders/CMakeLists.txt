cmake_minimum_required (VERSION 2.6)

set (SHADERS
   bbox.shader
   button.shader
   crate.shader
   level.shader
   physics.shader
   skybox.shader
)

set (PROCESSED_SHADERS)
foreach (SHADER ${SHADERS})
   set (PROCESSED_SHADER_PATH "${CMAKE_CURRENT_BINARY_DIR}/${SHADER}")
   add_custom_command (
      OUTPUT ${PROCESSED_SHADER_PATH}
      COMMAND converter -i "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER}" -o "${PROCESSED_SHADER_PATH}" ${MAKE_SILENT}
      DEPENDS ${SHADER} converter
   )
   list (APPEND PROCESSED_SHADERS ${PROCESSED_SHADER_PATH})
endforeach ()

add_custom_target (shaders
   DEPENDS ${PROCESSED_SHADERS}
)

install (FILES ${PROCESSED_SHADERS} DESTINATION data/shaders)

