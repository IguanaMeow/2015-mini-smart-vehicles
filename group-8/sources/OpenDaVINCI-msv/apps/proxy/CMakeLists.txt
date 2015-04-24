#
# Mini-Smart-Vehicles.
#
# This software is open source. Please see COPYING and AUTHORS for further information.
#

PROJECT (proxy)

# OpenCV libraries.
SET (OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui)

IF("${PANDABOARD}" STREQUAL "YES")
    SET (CMAKE_C_FLAGS "-pipe -ansi -D PANDABOARD=1")
    SET (CMAKE_CXX_FLAGS "-pipe -ansi -D PANDABOARD=1")

    INCLUDE_DIRECTORIES (/opt/PandaBoard-SD/usr/include /opt/PandaBoard-SD/usr/local/include)
    LINK_DIRECTORIES (/opt/PandaBoard-SD/lib /opt/PandaBoard-SD/lib/arm-linux-gnueabihf /opt/PandaBoard-SD/usr/lib /opt/PandaBoard-SD/usr/lib/arm-linux-gnueabihf /opt/PandaBoard-SD/usr/lib/arm-linux-gnueabi /opt/PandaBoard-SD/usr/lib/lapack /opt/PandaBoard-SD/usr/lib/libblas /opt/PandaBoard-SD/usr/local/lib)

    SET (LIBS ${LIBS} glib-2.0)
    SET (CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath-link=/opt/PandaBoard-SD/lib:/opt/PandaBoard-SD/lib/arm-linux-gnueabihf:/opt/PandaBoard-SD/usr/lib:/opt/PandaBoard-SD/lib/arm-linux-gnueabihf:/opt/PandaBoard-SD/usr/lib/arm-linux-gnueabihf:/opt/PandaBoard-SD/usr/lib:/opt/PandaBoard-SD/lib/arm-linux-gnueabi:/opt/PandaBoard-SD/usr/lib/arm-linux-gnueabi")
ENDIF("${PANDABOARD}" STREQUAL "YES")

# Include directories from core.
INCLUDE_DIRECTORIES (${libopendavinci_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES (${libdata_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES (include)
 
#include serial library 
INCLUDE_DIRECTORIES (serial/include)

set(serial_SRCS serial/src/serial.cc serial/include/serial/serial.h serial/include/serial/v8stdint.h)
if(UNIX)
    list(APPEND serial_SRCS serial/src/impl/unix.cc)
else()
    list(APPEND serial_SRCS serial/src/impl/win.cc)
endif()
## Add serial library
add_library(serial ${serial_SRCS})
if(UNIX AND NOT APPLE)
    target_link_libraries(serial rt)
endif()

# Recipe for building "proxy".
FILE(GLOB_RECURSE proxy-sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
ADD_LIBRARY (proxylib STATIC ${proxy-sources})
ADD_EXECUTABLE (proxy "${CMAKE_CURRENT_SOURCE_DIR}/apps/MainModule.cpp")
TARGET_LINK_LIBRARIES (proxy proxylib ${DATA_LIBS} ${OPENDAVINCI_LIBS} ${LIBS} ${OpenCV_LIBS} serial) 

# Recipe for installing "proxy".
INSTALL(TARGETS proxy RUNTIME DESTINATION bin) 

# Enable CxxTest for all available testsuites.
IF(CXXTEST_FOUND)
    FILE(GLOB proxy-testsuites "${CMAKE_CURRENT_SOURCE_DIR}/testsuites/*.h")
    
    FOREACH(testsuite ${proxy-testsuites})
        STRING(REPLACE "/" ";" testsuite-list ${testsuite})

        LIST(LENGTH testsuite-list len)
        MATH(EXPR lastItem "${len}-1")
        LIST(GET testsuite-list "${lastItem}" testsuite-short)

        CXXTEST_ADD_TEST(${testsuite-short}-TestSuite ${testsuite-short}-TestSuite.cpp ${testsuite})
        TARGET_LINK_LIBRARIES(${testsuite-short}-TestSuite proxylib ${DATA_LIBS} ${OPENDAVINCI_LIBS} ${LIBS} ${OpenCV_LIBS} serial)
    ENDFOREACH()
ENDIF(CXXTEST_FOUND)

