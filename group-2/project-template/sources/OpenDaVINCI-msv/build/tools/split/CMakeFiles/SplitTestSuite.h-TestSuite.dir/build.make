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
include tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/depend.make

# Include the progress variables for this target.
include tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/progress.make

# Include the compile flags for this target's objects.
include tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/flags.make

tools/split/SplitTestSuite.h-TestSuite.cpp: ../tools/split/testsuites/SplitTestSuite.h
	$(CMAKE_COMMAND) -E cmake_progress_report /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating SplitTestSuite.h-TestSuite.cpp"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/split && python /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/_buildtools/cxxtest/cxxtestgen.py --runner=XmlFilePrinter --runnerParam="\"/home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/split/SplitTestSuite.h-TestSuite.cpp-TestSuiteReport.xml\"" --have-eh -o /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/split/SplitTestSuite.h-TestSuite.cpp /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/tools/split/testsuites/SplitTestSuite.h

tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/SplitTestSuite.h-TestSuite.cpp.o: tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/flags.make
tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/SplitTestSuite.h-TestSuite.cpp.o: tools/split/SplitTestSuite.h-TestSuite.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/SplitTestSuite.h-TestSuite.cpp.o"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/split && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/SplitTestSuite.h-TestSuite.dir/SplitTestSuite.h-TestSuite.cpp.o -c /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/split/SplitTestSuite.h-TestSuite.cpp

tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/SplitTestSuite.h-TestSuite.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SplitTestSuite.h-TestSuite.dir/SplitTestSuite.h-TestSuite.cpp.i"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/split && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/split/SplitTestSuite.h-TestSuite.cpp > CMakeFiles/SplitTestSuite.h-TestSuite.dir/SplitTestSuite.h-TestSuite.cpp.i

tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/SplitTestSuite.h-TestSuite.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SplitTestSuite.h-TestSuite.dir/SplitTestSuite.h-TestSuite.cpp.s"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/split && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/split/SplitTestSuite.h-TestSuite.cpp -o CMakeFiles/SplitTestSuite.h-TestSuite.dir/SplitTestSuite.h-TestSuite.cpp.s

tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/SplitTestSuite.h-TestSuite.cpp.o.requires:
.PHONY : tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/SplitTestSuite.h-TestSuite.cpp.o.requires

tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/SplitTestSuite.h-TestSuite.cpp.o.provides: tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/SplitTestSuite.h-TestSuite.cpp.o.requires
	$(MAKE) -f tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/build.make tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/SplitTestSuite.h-TestSuite.cpp.o.provides.build
.PHONY : tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/SplitTestSuite.h-TestSuite.cpp.o.provides

tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/SplitTestSuite.h-TestSuite.cpp.o.provides.build: tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/SplitTestSuite.h-TestSuite.cpp.o

# Object files for target SplitTestSuite.h-TestSuite
SplitTestSuite_h__TestSuite_OBJECTS = \
"CMakeFiles/SplitTestSuite.h-TestSuite.dir/SplitTestSuite.h-TestSuite.cpp.o"

# External object files for target SplitTestSuite.h-TestSuite
SplitTestSuite_h__TestSuite_EXTERNAL_OBJECTS =

tools/split/SplitTestSuite.h-TestSuite: tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/SplitTestSuite.h-TestSuite.cpp.o
tools/split/SplitTestSuite.h-TestSuite: tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/build.make
tools/split/SplitTestSuite.h-TestSuite: tools/split/libsplitlib.a
tools/split/SplitTestSuite.h-TestSuite: libopendavinci/libopendavinci.a
tools/split/SplitTestSuite.h-TestSuite: 3rdParty/libzip-0.9/libziplib.a
tools/split/SplitTestSuite.h-TestSuite: 3rdParty/zlib-1.2.3/libzlib.a
tools/split/SplitTestSuite.h-TestSuite: 3rdParty/libdb-4.6.21/libdblib.a
tools/split/SplitTestSuite.h-TestSuite: tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable SplitTestSuite.h-TestSuite"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/split && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/SplitTestSuite.h-TestSuite.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/build: tools/split/SplitTestSuite.h-TestSuite
.PHONY : tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/build

tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/requires: tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/SplitTestSuite.h-TestSuite.cpp.o.requires
.PHONY : tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/requires

tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/clean:
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/split && $(CMAKE_COMMAND) -P CMakeFiles/SplitTestSuite.h-TestSuite.dir/cmake_clean.cmake
.PHONY : tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/clean

tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/depend: tools/split/SplitTestSuite.h-TestSuite.cpp
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/tools/split /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/split /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tools/split/CMakeFiles/SplitTestSuite.h-TestSuite.dir/depend

