# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build

# Include any dependencies generated for this target.
include libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/depend.make

# Include the progress variables for this target.
include libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/progress.make

# Include the compile flags for this target's objects.
include libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/flags.make

libdata/msv_SteeringDataTestSuite.h-TestSuite.cpp: ../libdata/testsuites/msv_SteeringDataTestSuite.h
	$(CMAKE_COMMAND) -E cmake_progress_report /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating msv_SteeringDataTestSuite.h-TestSuite.cpp"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libdata && python /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/_buildtools/cxxtest/cxxtestgen.py --runner=XmlFilePrinter --runnerParam="\"/home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libdata/msv_SteeringDataTestSuite.h-TestSuite.cpp-TestSuiteReport.xml\"" --have-eh -o /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libdata/msv_SteeringDataTestSuite.h-TestSuite.cpp /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/libdata/testsuites/msv_SteeringDataTestSuite.h

libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/msv_SteeringDataTestSuite.h-TestSuite.cpp.o: libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/flags.make
libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/msv_SteeringDataTestSuite.h-TestSuite.cpp.o: libdata/msv_SteeringDataTestSuite.h-TestSuite.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/msv_SteeringDataTestSuite.h-TestSuite.cpp.o"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libdata && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/msv_SteeringDataTestSuite.h-TestSuite.cpp.o -c /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libdata/msv_SteeringDataTestSuite.h-TestSuite.cpp

libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/msv_SteeringDataTestSuite.h-TestSuite.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/msv_SteeringDataTestSuite.h-TestSuite.cpp.i"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libdata && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libdata/msv_SteeringDataTestSuite.h-TestSuite.cpp > CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/msv_SteeringDataTestSuite.h-TestSuite.cpp.i

libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/msv_SteeringDataTestSuite.h-TestSuite.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/msv_SteeringDataTestSuite.h-TestSuite.cpp.s"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libdata && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libdata/msv_SteeringDataTestSuite.h-TestSuite.cpp -o CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/msv_SteeringDataTestSuite.h-TestSuite.cpp.s

libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/msv_SteeringDataTestSuite.h-TestSuite.cpp.o.requires:
.PHONY : libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/msv_SteeringDataTestSuite.h-TestSuite.cpp.o.requires

libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/msv_SteeringDataTestSuite.h-TestSuite.cpp.o.provides: libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/msv_SteeringDataTestSuite.h-TestSuite.cpp.o.requires
	$(MAKE) -f libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/build.make libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/msv_SteeringDataTestSuite.h-TestSuite.cpp.o.provides.build
.PHONY : libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/msv_SteeringDataTestSuite.h-TestSuite.cpp.o.provides

libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/msv_SteeringDataTestSuite.h-TestSuite.cpp.o.provides.build: libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/msv_SteeringDataTestSuite.h-TestSuite.cpp.o

# Object files for target msv_SteeringDataTestSuite.h-TestSuite
msv_SteeringDataTestSuite_h__TestSuite_OBJECTS = \
"CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/msv_SteeringDataTestSuite.h-TestSuite.cpp.o"

# External object files for target msv_SteeringDataTestSuite.h-TestSuite
msv_SteeringDataTestSuite_h__TestSuite_EXTERNAL_OBJECTS =

libdata/msv_SteeringDataTestSuite.h-TestSuite: libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/msv_SteeringDataTestSuite.h-TestSuite.cpp.o
libdata/msv_SteeringDataTestSuite.h-TestSuite: libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/build.make
libdata/msv_SteeringDataTestSuite.h-TestSuite: libdata/libdata.a
libdata/msv_SteeringDataTestSuite.h-TestSuite: libopendavinci/libopendavinci.a
libdata/msv_SteeringDataTestSuite.h-TestSuite: 3rdParty/libzip-0.9/libziplib.a
libdata/msv_SteeringDataTestSuite.h-TestSuite: 3rdParty/zlib-1.2.3/libzlib.a
libdata/msv_SteeringDataTestSuite.h-TestSuite: 3rdParty/libdb-4.6.21/libdblib.a
libdata/msv_SteeringDataTestSuite.h-TestSuite: libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable msv_SteeringDataTestSuite.h-TestSuite"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libdata && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/build: libdata/msv_SteeringDataTestSuite.h-TestSuite
.PHONY : libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/build

libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/requires: libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/msv_SteeringDataTestSuite.h-TestSuite.cpp.o.requires
.PHONY : libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/requires

libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/clean:
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libdata && $(CMAKE_COMMAND) -P CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/cmake_clean.cmake
.PHONY : libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/clean

libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/depend: libdata/msv_SteeringDataTestSuite.h-TestSuite.cpp
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/libdata /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libdata /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : libdata/CMakeFiles/msv_SteeringDataTestSuite.h-TestSuite.dir/depend

