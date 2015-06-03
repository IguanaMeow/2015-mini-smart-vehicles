#
# OpenDaVINCI.
#
# This software is open source. Please see COPYING and AUTHORS for further information.
#

PROJECT (joy_bringer)

SET(SDL2_INCLUDE_PATH    "/usr/include/SDL2/")

# Include directories from core.
INCLUDE_DIRECTORIES (${libopendavinci_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES (${libdata_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES (include)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_PATH})

# Recipe for building "serial_killer".
FILE(GLOB_RECURSE joy_bringer-sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
FILE(GLOB SDL2_LIB "/usr/lib/libSDL2.so")
ADD_EXECUTABLE (joy_bringer ${joy_bringer-sources})
TARGET_LINK_LIBRARIES (joy_bringer ${DATA_LIBS} ${OPENDAVINCI_LIBS} ${LIBS} ${SDL2_LIB}) 

# Recipe for installing "serial_killer".
INSTALL(TARGETS joy_bringer RUNTIME DESTINATION bin) 
