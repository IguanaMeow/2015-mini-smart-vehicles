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
include hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/depend.make

# Include the progress variables for this target.
include hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/progress.make

# Include the compile flags for this target's objects.
include hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/flags.make

hesperia/libhesperia/QuaternionTestSuite.h-TestSuite.cpp: ../hesperia/libhesperia/testsuites/QuaternionTestSuite.h
	$(CMAKE_COMMAND) -E cmake_progress_report /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating QuaternionTestSuite.h-TestSuite.cpp"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/hesperia/libhesperia && python /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/_buildtools/cxxtest/cxxtestgen.py --runner=XmlFilePrinter --runnerParam="\"/home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/hesperia/libhesperia/QuaternionTestSuite.h-TestSuite.cpp-TestSuiteReport.xml\"" --have-eh -o /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/hesperia/libhesperia/QuaternionTestSuite.h-TestSuite.cpp /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/hesperia/libhesperia/testsuites/QuaternionTestSuite.h

hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/QuaternionTestSuite.h-TestSuite.cpp.o: hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/flags.make
hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/QuaternionTestSuite.h-TestSuite.cpp.o: hesperia/libhesperia/QuaternionTestSuite.h-TestSuite.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/QuaternionTestSuite.h-TestSuite.cpp.o"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/hesperia/libhesperia && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/QuaternionTestSuite.h-TestSuite.cpp.o -c /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/hesperia/libhesperia/QuaternionTestSuite.h-TestSuite.cpp

hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/QuaternionTestSuite.h-TestSuite.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/QuaternionTestSuite.h-TestSuite.cpp.i"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/hesperia/libhesperia && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/hesperia/libhesperia/QuaternionTestSuite.h-TestSuite.cpp > CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/QuaternionTestSuite.h-TestSuite.cpp.i

hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/QuaternionTestSuite.h-TestSuite.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/QuaternionTestSuite.h-TestSuite.cpp.s"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/hesperia/libhesperia && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/hesperia/libhesperia/QuaternionTestSuite.h-TestSuite.cpp -o CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/QuaternionTestSuite.h-TestSuite.cpp.s

hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/QuaternionTestSuite.h-TestSuite.cpp.o.requires:
.PHONY : hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/QuaternionTestSuite.h-TestSuite.cpp.o.requires

hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/QuaternionTestSuite.h-TestSuite.cpp.o.provides: hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/QuaternionTestSuite.h-TestSuite.cpp.o.requires
	$(MAKE) -f hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/build.make hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/QuaternionTestSuite.h-TestSuite.cpp.o.provides.build
.PHONY : hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/QuaternionTestSuite.h-TestSuite.cpp.o.provides

hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/QuaternionTestSuite.h-TestSuite.cpp.o.provides.build: hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/QuaternionTestSuite.h-TestSuite.cpp.o

# Object files for target QuaternionTestSuite.h-TestSuite
QuaternionTestSuite_h__TestSuite_OBJECTS = \
"CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/QuaternionTestSuite.h-TestSuite.cpp.o"

# External object files for target QuaternionTestSuite.h-TestSuite
QuaternionTestSuite_h__TestSuite_EXTERNAL_OBJECTS =

hesperia/libhesperia/QuaternionTestSuite.h-TestSuite: hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/QuaternionTestSuite.h-TestSuite.cpp.o
hesperia/libhesperia/QuaternionTestSuite.h-TestSuite: hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/build.make
hesperia/libhesperia/QuaternionTestSuite.h-TestSuite: hesperia/libhesperia/libhesperia.a
hesperia/libhesperia/QuaternionTestSuite.h-TestSuite: libopendavinci/libopendavinci.a
hesperia/libhesperia/QuaternionTestSuite.h-TestSuite: 3rdParty/libzip-0.9/libziplib.a
hesperia/libhesperia/QuaternionTestSuite.h-TestSuite: 3rdParty/zlib-1.2.3/libzlib.a
hesperia/libhesperia/QuaternionTestSuite.h-TestSuite: 3rdParty/libdb-4.6.21/libdblib.a
hesperia/libhesperia/QuaternionTestSuite.h-TestSuite: hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable QuaternionTestSuite.h-TestSuite"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/hesperia/libhesperia && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/build: hesperia/libhesperia/QuaternionTestSuite.h-TestSuite
.PHONY : hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/build

hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/requires: hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/QuaternionTestSuite.h-TestSuite.cpp.o.requires
.PHONY : hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/requires

hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/clean:
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/hesperia/libhesperia && $(CMAKE_COMMAND) -P CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/cmake_clean.cmake
.PHONY : hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/clean

hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/depend: hesperia/libhesperia/QuaternionTestSuite.h-TestSuite.cpp
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/hesperia/libhesperia /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/hesperia/libhesperia /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : hesperia/libhesperia/CMakeFiles/QuaternionTestSuite.h-TestSuite.dir/depend

