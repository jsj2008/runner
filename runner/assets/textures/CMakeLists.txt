cmake_minimum_required (VERSION 2.6)

find_package (ImageMagick REQUIRED)

set (RGB_TEXTURES
   barrel_rusty01.png
   crate01.png
   grass06.png
   skybox.png
)

set (RGBA_TEXTURES
   buttons.png 
)

macro (compress_textures
   TEXTURES
   S3TC_PARAMS
   PVRTC_PARAMS
   ATC_PARAMS
   ETC_PARAMS
)
   foreach (_file ${TEXTURES})
      set (FLIPPED_TEXTURE "${CMAKE_CURRENT_BINARY_DIR}/${_file}")
      add_custom_command (
         OUTPUT ${FLIPPED_TEXTURE}
         COMMAND ${IMAGEMAGICK_CONVERT_EXECUTABLE} -flip "${CMAKE_CURRENT_SOURCE_DIR}/${_file}" "${FLIPPED_TEXTURE}"
         DEPENDS ${_file}
      )

      if (NOT "${S3TC_PARAMS}" STREQUAL "")
         # s3tc compression
         string (REPLACE ".png" ".dds" OUT_NAME ${_file})
         set (OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/s3tc")
         file (MAKE_DIRECTORY "${OUT_DIR}")
         set (OUT_PATH "${OUT_DIR}/${OUT_NAME}")
         add_custom_command (
            OUTPUT ${OUT_PATH}
            COMMAND nvcompress ${S3TC_PARAMS} "${FLIPPED_TEXTURE}" "${OUT_PATH}" >/dev/null
            DEPENDS ${FLIPPED_TEXTURE} ${OUT_DIR}
         )
         list (APPEND COMPRESSED_TEXTURES ${OUT_PATH})
      endif (NOT "${S3TC_PARAMS}" STREQUAL "")

      if (NOT "${PVRTC_PARAMS}" STREQUAL "")
         # pvrtc compression
         string (REPLACE ".png" ".pvr" OUT_NAME ${_file})
         set (OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/pvrtc")
         file (MAKE_DIRECTORY "${OUT_DIR}")
         set (OUT_PATH "${OUT_DIR}/${OUT_NAME}")
         add_custom_command (
            OUTPUT ${OUT_PATH}
            COMMAND ${PROJECT_SOURCE_DIR}/../tools/PVRTexTool ${PVRTC_PARAMS} -i "${FLIPPED_TEXTURE}" -o "${OUT_PATH}" >/dev/null
            DEPENDS ${FLIPPED_TEXTURE} ${OUT_DIR}
         )
         list (APPEND COMPRESSED_TEXTURES ${OUT_PATH})
      endif (NOT "${PVRTC_PARAMS}" STREQUAL "")

      if (NOT "${ETC_PARAMS}" STREQUAL "")
         # etc compression
         string (REPLACE ".png" ".pvr" OUT_NAME ${_file})
         set (OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/etc")
         file (MAKE_DIRECTORY "${OUT_DIR}")
         set (OUT_PATH "${OUT_DIR}/${OUT_NAME}")
         add_custom_command (
            OUTPUT ${OUT_PATH}
            COMMAND ${PROJECT_SOURCE_DIR}/../tools/PVRTexTool ${ETC_PARAMS} -i "${FLIPPED_TEXTURE}" -o "${OUT_PATH}" >/dev/null
            DEPENDS ${FLIPPED_TEXTURE} ${OUT_DIR}
         )
         list (APPEND COMPRESSED_TEXTURES ${OUT_PATH})
      endif (NOT "${ETC_PARAMS}" STREQUAL "")
   endforeach ()
endmacro (compress_textures)

set (COMPRESSED_TEXTURES)

compress_textures (
   "${RGB_TEXTURES}"
   "-color;-bc3"
   "-fPVRTC2;-m;-yflip0;-silent"
   ""
   "-fETC;-m;-yflip0;-silent"
)

compress_textures (
   "${RGBA_TEXTURES}"
   "-alpha;-bc3"
   "-fPVRTC4;-m;-yflip0;-silent"
   ""
   ""
)

set (CONVERTED_TEXTURES)
foreach (_file ${COMPRESSED_TEXTURES})
   string (REPLACE ".dds" ".texture" CONVERTED_TEXTURE "${_file}")
   string (REPLACE ".pvr" ".texture" CONVERTED_TEXTURE "${CONVERTED_TEXTURE}")
   add_custom_command (
      OUTPUT ${CONVERTED_TEXTURE}
      COMMAND converter -i "${_file}" -o "${CONVERTED_TEXTURE}" >/dev/null
      DEPENDS ${_file} converter
   )
   list (APPEND CONVERTED_TEXTURES ${CONVERTED_TEXTURE})

   get_filename_component (FILE_PATH ${CONVERTED_TEXTURE} PATH)
   get_filename_component (DIR_NAME ${FILE_PATH} NAME)
   install (FILES ${CONVERTED_TEXTURE} DESTINATION data/textures/${DIR_NAME})
endforeach()

add_custom_target (textures
   DEPENDS ${CONVERTED_TEXTURES}
)

install (FILES ${RGB_TEXTURES} ${RGBA_TEXTURES} DESTINATION data/textures)

