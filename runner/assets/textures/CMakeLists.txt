cmake_minimum_required (VERSION 2.6)

find_package (ImageMagick COMPONENTS convert REQUIRED)

set (RGB_TEXTURES
   barrel_rusty01.png
   crate01.png
   grass06.png
   skybox.png
)

set (RGBA_TEXTURES
   buttons.png 
)

if (UNIX)
   set (ATC_COMPRESSOR "")
   set (S3TC_COMPRESSOR "nvcompress")
   set (PVRTC_COMPRESSOR "${PROJECT_SOURCE_DIR}/../tools/PVRTexTool")
   set (ETC_COMPRESSOR "${PROJECT_SOURCE_DIR}/../tools/PVRTexTool")
elseif (WIN32)
   set (ATC_COMPRESSOR "${PROJECT_SOURCE_DIR}/../tools/TheCompressonator.exe")
   set (S3TC_COMPRESSOR "${PROJECT_SOURCE_DIR}/../tools/nvcompress.exe")
   set (PVRTC_COMPRESSOR "${PROJECT_SOURCE_DIR}/../tools/PVRTexTool.exe")
   set (ETC_COMPRESSOR "${PROJECT_SOURCE_DIR}/../tools/PVRTexTool.exe")
endif()

function (compress_texture
   TEXTURE
   COMPRESSOR
   COMPRESSOR_PARAMS
   OUT_DIR_NAME
   OUT_EXTENSION
   COMPRESSED_TEXTURES
)
   if (NOT ("${COMPRESSOR_PARAMS}" STREQUAL "" OR "${COMPRESSOR}" STREQUAL ""))
      get_filename_component (IN_NAME ${TEXTURE} NAME)
      string (REPLACE ".png" "${OUT_EXTENSION}" OUT_NAME ${IN_NAME})
      set (OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/${OUT_DIR_NAME}")
      file (MAKE_DIRECTORY "${OUT_DIR}")
      set (OUT_PATH "${OUT_DIR}/${OUT_NAME}")
      set (IN_PATH "${TEXTURE}")
      string (CONFIGURE "${COMPRESSOR_PARAMS}" PARAMS)
      separate_arguments (PARAMS)
      #message ("${PARAMS}")
      add_custom_command (
         OUTPUT ${OUT_PATH}
         COMMAND ${COMPRESSOR} ${PARAMS} ${MAKE_SILENT}
         DEPENDS ${TEXTURE}
      )
      list (APPEND COMPRESSED_TEXTURES ${OUT_PATH})
      set (COMPRESSED_TEXTURES ${COMPRESSED_TEXTURES} PARENT_SCOPE)
   endif (NOT ("${COMPRESSOR_PARAMS}" STREQUAL "" OR "${COMPRESSOR}" STREQUAL ""))
endfunction (compress_texture)

macro (compress_textures
   TEXTURES
   ATC_PARAMS
   S3TC_PARAMS
   PVRTC_PARAMS
   ETC_PARAMS
)
   foreach (_file ${TEXTURES})
      set (FLIPPED_TEXTURE "${CMAKE_CURRENT_BINARY_DIR}/${_file}")
      add_custom_command (
         OUTPUT ${FLIPPED_TEXTURE}
         COMMAND ${IMAGEMAGICK_CONVERT_EXECUTABLE} -flip "${CMAKE_CURRENT_SOURCE_DIR}/${_file}" "${FLIPPED_TEXTURE}"
         DEPENDS ${_file}
      )

      compress_texture (
         "${FLIPPED_TEXTURE}"
         "${ATC_COMPRESSOR}"
         "${ATC_PARAMS}"
         "atc"
         ".dds"
         "${COMPRESSED_TEXTURES}"
      )

      compress_texture (
         "${FLIPPED_TEXTURE}"
         "${S3TC_COMPRESSOR}"
         "${S3TC_PARAMS}"
         "s3tc"
         ".dds"
         "${COMPRESSED_TEXTURES}"
      )

      compress_texture (
         "${FLIPPED_TEXTURE}"
         "${PVRTC_COMPRESSOR}"
         "${PVRTC_PARAMS}"
         "pvrtc"
         ".pvr"
         "${COMPRESSED_TEXTURES}"
      )

      compress_texture (
         "${FLIPPED_TEXTURE}"
         "${ETC_COMPRESSOR}"
         "${ETC_PARAMS}"
         "etc"
         ".pvr"
         "${COMPRESSED_TEXTURES}"
      )
   endforeach ()
endmacro (compress_textures)

set (COMPRESSED_TEXTURES)

compress_textures (
   "${RGB_TEXTURES}"
   "\"@IN_PATH@\" \"@OUT_PATH@\""
   "-color -bc3 \"@IN_PATH@\" \"@OUT_PATH@\""
   "-fPVRTC2 -m -yflip0 -silent -i \"@IN_PATH@\" -o \"@OUT_PATH@\""
   "-fETC -m -yflip0 -silent -i \"@IN_PATH@\" -o \"@OUT_PATH@\""
)

compress_textures (
   "${RGBA_TEXTURES}"
   "\"@IN_PATH@\" \"@OUT_PATH@\""
   "-alpha -bc3 \"@IN_PATH@\" \"@OUT_PATH@\""
   "-fPVRTC4 -m -yflip0 -silent -i \"@IN_PATH@\" -o \"@OUT_PATH@\""
   ""
)

set (CONVERTED_TEXTURES)
foreach (_file ${COMPRESSED_TEXTURES})
   string (REPLACE ".dds" ".texture" CONVERTED_TEXTURE "${_file}")
   string (REPLACE ".pvr" ".texture" CONVERTED_TEXTURE "${CONVERTED_TEXTURE}")
   add_custom_command (
      OUTPUT ${CONVERTED_TEXTURE}
      COMMAND converter -i "${_file}" -o "${CONVERTED_TEXTURE}" ${MAKE_SILENT}
      DEPENDS ${_file} converter
   )
   list (APPEND CONVERTED_TEXTURES ${CONVERTED_TEXTURE})

   get_filename_component (FILE_PATH ${CONVERTED_TEXTURE} PATH)
   get_filename_component (DIR_NAME ${FILE_PATH} NAME)
   install (FILES ${CONVERTED_TEXTURE} DESTINATION data/textures/${DIR_NAME})
endforeach()

add_custom_target (textures
   DEPENDS ${CONVERTED_TEXTURES}
   SOURCES ${RGB_TEXTURES} ${RGBA_TEXTURES}
)

install (FILES ${RGB_TEXTURES} ${RGBA_TEXTURES} DESTINATION data/textures)

