cmake_minimum_required (VERSION 2.6)

add_library (backtrace-mingw MODULE
   backtrace.c
)

target_link_libraries (backtrace-mingw bfd iberty imagehlp intl)

install (TARGETS backtrace-mingw DESTINATION bin)
