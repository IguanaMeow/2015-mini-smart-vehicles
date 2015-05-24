#
# OpenDaVINCI.
#
# This software is open source. Please see COPYING and AUTHORS for further information.
#

PROJECT (serial_killer)

# Include directories from core.
INCLUDE_DIRECTORIES (${libopendavinci_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES (${libdata_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES (include)

# Recipe for building "serial_killer".
FILE(GLOB_RECURSE serial_killer-sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
ADD_EXECUTABLE (serial_killer ${serial_killer-sources})
TARGET_LINK_LIBRARIES (serial_killer ${DATA_LIBS} ${OPENDAVINCI_LIBS} ${LIBS}) 

# Recipe for installing "serial_killer".
INSTALL(TARGETS serial_killer RUNTIME DESTINATION bin) 
