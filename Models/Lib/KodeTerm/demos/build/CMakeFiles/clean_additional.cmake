# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "CMakeFiles\\KodoShell_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\KodoShell_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\KodoTermSimple_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\KodoTermSimple_autogen.dir\\ParseCache.txt"
  "KodoShell_autogen"
  "KodoTermSimple_autogen"
  )
endif()
