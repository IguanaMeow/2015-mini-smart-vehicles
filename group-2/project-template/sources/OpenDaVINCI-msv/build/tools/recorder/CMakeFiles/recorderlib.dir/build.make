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
include tools/recorder/CMakeFiles/recorderlib.dir/depend.make

# Include the progress variables for this target.
include tools/recorder/CMakeFiles/recorderlib.dir/progress.make

# Include the compile flags for this target's objects.
include tools/recorder/CMakeFiles/recorderlib.dir/flags.make

tools/recorder/CMakeFiles/recorderlib.dir/src/RecorderModule.cpp.o: tools/recorder/CMakeFiles/recorderlib.dir/flags.make
tools/recorder/CMakeFiles/recorderlib.dir/src/RecorderModule.cpp.o: ../tools/recorder/src/RecorderModule.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object tools/recorder/CMakeFiles/recorderlib.dir/src/RecorderModule.cpp.o"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/recorder && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/recorderlib.dir/src/RecorderModule.cpp.o -c /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/tools/recorder/src/RecorderModule.cpp

tools/recorder/CMakeFiles/recorderlib.dir/src/RecorderModule.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/recorderlib.dir/src/RecorderModule.cpp.i"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/recorder && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/tools/recorder/src/RecorderModule.cpp > CMakeFiles/recorderlib.dir/src/RecorderModule.cpp.i

tools/recorder/CMakeFiles/recorderlib.dir/src/RecorderModule.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/recorderlib.dir/src/RecorderModule.cpp.s"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/recorder && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/tools/recorder/src/RecorderModule.cpp -o CMakeFiles/recorderlib.dir/src/RecorderModule.cpp.s

tools/recorder/CMakeFiles/recorderlib.dir/src/RecorderModule.cpp.o.requires:
.PHONY : tools/recorder/CMakeFiles/recorderlib.dir/src/RecorderModule.cpp.o.requires

tools/recorder/CMakeFiles/recorderlib.dir/src/RecorderModule.cpp.o.provides: tools/recorder/CMakeFiles/recorderlib.dir/src/RecorderModule.cpp.o.requires
	$(MAKE) -f tools/recorder/CMakeFiles/recorderlib.dir/build.make tools/recorder/CMakeFiles/recorderlib.dir/src/RecorderModule.cpp.o.provides.build
.PHONY : tools/recorder/CMakeFiles/recorderlib.dir/src/RecorderModule.cpp.o.provides

tools/recorder/CMakeFiles/recorderlib.dir/src/RecorderModule.cpp.o.provides.build: tools/recorder/CMakeFiles/recorderlib.dir/src/RecorderModule.cpp.o

# Object files for target recorderlib
recorderlib_OBJECTS = \
"CMakeFiles/recorderlib.dir/src/RecorderModule.cpp.o"

# External object files for target recorderlib
recorderlib_EXTERNAL_OBJECTS =

tools/recorder/librecorderlib.a: tools/recorder/CMakeFiles/recorderlib.dir/src/RecorderModule.cpp.o
tools/recorder/librecorderlib.a: tools/recorder/CMakeFiles/recorderlib.dir/build.make
tools/recorder/librecorderlib.a: tools/recorder/CMakeFiles/recorderlib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX static library librecorderlib.a"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/recorder && $(CMAKE_COMMAND) -P CMakeFiles/recorderlib.dir/cmake_clean_target.cmake
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/recorder && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/recorderlib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tools/recorder/CMakeFiles/recorderlib.dir/build: tools/recorder/librecorderlib.a
.PHONY : tools/recorder/CMakeFiles/recorderlib.dir/build

tools/recorder/CMakeFiles/recorderlib.dir/requires: tools/recorder/CMakeFiles/recorderlib.dir/src/RecorderModule.cpp.o.requires
.PHONY : tools/recorder/CMakeFiles/recorderlib.dir/requires

tools/recorder/CMakeFiles/recorderlib.dir/clean:
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/recorder && $(CMAKE_COMMAND) -P CMakeFiles/recorderlib.dir/cmake_clean.cmake
.PHONY : tools/recorder/CMakeFiles/recorderlib.dir/clean

tools/recorder/CMakeFiles/recorderlib.dir/depend:
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/tools/recorder /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/recorder /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/recorder/CMakeFiles/recorderlib.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tools/recorder/CMakeFiles/recorderlib.dir/depend

