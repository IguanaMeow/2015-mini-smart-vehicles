# CMake generated Testfile for 
# Source directory: /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/apps/lanedetector
# Build directory: /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/apps/lanedetector
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
ADD_TEST(LaneDetectorTestSuite.h-TestSuite "/home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/apps/lanedetector/LaneDetectorTestSuite.h-TestSuite")
SET_TESTS_PROPERTIES(LaneDetectorTestSuite.h-TestSuite PROPERTIES  TIMEOUT "1800")
ADD_TEST(LaneDetectorReplayTestsuite.h-TestSuite "/home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/apps/lanedetector/LaneDetectorReplayTestsuite.h-TestSuite")
SET_TESTS_PROPERTIES(LaneDetectorReplayTestsuite.h-TestSuite PROPERTIES  TIMEOUT "1800")
