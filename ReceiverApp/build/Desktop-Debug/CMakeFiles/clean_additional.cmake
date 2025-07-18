# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/ReceiverApp_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/ReceiverApp_autogen.dir/ParseCache.txt"
  "ReceiverApp_autogen"
  )
endif()
