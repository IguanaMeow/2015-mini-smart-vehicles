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
include libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/depend.make

# Include the progress variables for this target.
include libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/progress.make

# Include the compile flags for this target's objects.
include libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/flags.make

libopendavinci/StringProtocolTestSuite.h-TestSuite.cpp: ../libopendavinci/testsuites/StringProtocolTestSuite.h
	$(CMAKE_COMMAND) -E cmake_progress_report /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating StringProtocolTestSuite.h-TestSuite.cpp"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libopendavinci && python /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/_buildtools/cxxtest/cxxtestgen.py --runner=XmlFilePrinter --runnerParam="\"/home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libopendavinci/StringProtocolTestSuite.h-TestSuite.cpp-TestSuiteReport.xml\"" --have-eh -o /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libopendavinci/StringProtocolTestSuite.h-TestSuite.cpp /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/libopendavinci/testsuites/StringProtocolTestSuite.h

libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/StringProtocolTestSuite.h-TestSuite.cpp.o: libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/flags.make
libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/StringProtocolTestSuite.h-TestSuite.cpp.o: libopendavinci/StringProtocolTestSuite.h-TestSuite.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/StringProtocolTestSuite.h-TestSuite.cpp.o"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libopendavinci && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/StringProtocolTestSuite.h-TestSuite.cpp.o -c /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libopendavinci/StringProtocolTestSuite.h-TestSuite.cpp

libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/StringProtocolTestSuite.h-TestSuite.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/StringProtocolTestSuite.h-TestSuite.cpp.i"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libopendavinci && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libopendavinci/StringProtocolTestSuite.h-TestSuite.cpp > CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/StringProtocolTestSuite.h-TestSuite.cpp.i

libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/StringProtocolTestSuite.h-TestSuite.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/StringProtocolTestSuite.h-TestSuite.cpp.s"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libopendavinci && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libopendavinci/StringProtocolTestSuite.h-TestSuite.cpp -o CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/StringProtocolTestSuite.h-TestSuite.cpp.s

libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/StringProtocolTestSuite.h-TestSuite.cpp.o.requires:
.PHONY : libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/StringProtocolTestSuite.h-TestSuite.cpp.o.requires

libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/StringProtocolTestSuite.h-TestSuite.cpp.o.provides: libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/StringProtocolTestSuite.h-TestSuite.cpp.o.requires
	$(MAKE) -f libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/build.make libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/StringProtocolTestSuite.h-TestSuite.cpp.o.provides.build
.PHONY : libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/StringProtocolTestSuite.h-TestSuite.cpp.o.provides

libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/StringProtocolTestSuite.h-TestSuite.cpp.o.provides.build: libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/StringProtocolTestSuite.h-TestSuite.cpp.o

# Object files for target StringProtocolTestSuite.h-TestSuite
StringProtocolTestSuite_h__TestSuite_OBJECTS = \
"CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/StringProtocolTestSuite.h-TestSuite.cpp.o"

# External object files for target StringProtocolTestSuite.h-TestSuite
StringProtocolTestSuite_h__TestSuite_EXTERNAL_OBJECTS =

libopendavinci/StringProtocolTestSuite.h-TestSuite: libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/StringProtocolTestSuite.h-TestSuite.cpp.o
libopendavinci/StringProtocolTestSuite.h-TestSuite: libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/build.make
libopendavinci/StringProtocolTestSuite.h-TestSuite: libopendavinci/libopendavinci.a
libopendavinci/StringProtocolTestSuite.h-TestSuite: libopendavinci/libopendavinci.a
libopendavinci/StringProtocolTestSuite.h-TestSuite: 3rdParty/libzip-0.9/libziplib.a
libopendavinci/StringProtocolTestSuite.h-TestSuite: 3rdParty/zlib-1.2.3/libzlib.a
libopendavinci/StringProtocolTestSuite.h-TestSuite: 3rdParty/libdb-4.6.21/libdblib.a
libopendavinci/StringProtocolTestSuite.h-TestSuite: libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable StringProtocolTestSuite.h-TestSuite"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libopendavinci && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/build: libopendavinci/StringProtocolTestSuite.h-TestSuite
.PHONY : libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/build

libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/requires: libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/StringProtocolTestSuite.h-TestSuite.cpp.o.requires
.PHONY : libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/requires

libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/clean:
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libopendavinci && $(CMAKE_COMMAND) -P CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/cmake_clean.cmake
.PHONY : libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/clean

libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/depend: libopendavinci/StringProtocolTestSuite.h-TestSuite.cpp
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/libopendavinci /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libopendavinci /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : libopendavinci/CMakeFiles/StringProtocolTestSuite.h-TestSuite.dir/depend

