# Mini-Smart-Vehicles sources.
# Disable QT and OpenCV apps on platforms missing those libraries.
IF((NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin") AND (NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "FreeBSD") AND (NOT WIN32) AND (NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "NetBSD") AND (NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "OpenBSD"))
    # Application for receiving data from hesperia or a USB camera.
    ADD_SUBDIRECTORY (lanedetector)

    # Application for connecting to a serial embedded software/hardware board (STM32F4 Discovery Board or Arduino).
    ADD_SUBDIRECTORY (proxy)

    # Application to compute steering wheel values and acceleration/braking values.
    ADD_SUBDIRECTORY (driver)

    # Application for parking logic
    ADD_SUBDIRECTORY (parker)

    # Application for overtaking logic
    ADD_SUBDIRECTORY (overtaker)

    # Application for recording a video of the driving performance.
    ADD_SUBDIRECTORY (VCR)
ENDIF()

IF("${PANDABOARD}" STREQUAL "YES")
ADD_CUSTOM_TARGET(push2meili-1
    COMMAND rsync -ave ssh --rsync-path='sudo rsync' /opt/msv-ARM/ odroid@192.168.1.137:/opt/msv/
    DEPENDS lanedetector driver proxy
)
ENDIF("${PANDABOARD}" STREQUAL "YES")

