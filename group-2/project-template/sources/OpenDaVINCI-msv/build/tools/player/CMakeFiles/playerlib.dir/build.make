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
include tools/player/CMakeFiles/playerlib.dir/depend.make

# Include the progress variables for this target.
include tools/player/CMakeFiles/playerlib.dir/progress.make

# Include the compile flags for this target's objects.
include tools/player/CMakeFiles/playerlib.dir/flags.make

tools/player/CMakeFiles/playerlib.dir/src/PlayerModule.cpp.o: tools/player/CMakeFiles/playerlib.dir/flags.make
tools/player/CMakeFiles/playerlib.dir/src/PlayerModule.cpp.o: ../tools/player/src/PlayerModule.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object tools/player/CMakeFiles/playerlib.dir/src/PlayerModule.cpp.o"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/player && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/playerlib.dir/src/PlayerModule.cpp.o -c /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/tools/player/src/PlayerModule.cpp

tools/player/CMakeFiles/playerlib.dir/src/PlayerModule.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/playerlib.dir/src/PlayerModule.cpp.i"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/player && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/tools/player/src/PlayerModule.cpp > CMakeFiles/playerlib.dir/src/PlayerModule.cpp.i

tools/player/CMakeFiles/playerlib.dir/src/PlayerModule.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/playerlib.dir/src/PlayerModule.cpp.s"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/player && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/tools/player/src/PlayerModule.cpp -o CMakeFiles/playerlib.dir/src/PlayerModule.cpp.s

tools/player/CMakeFiles/playerlib.dir/src/PlayerModule.cpp.o.requires:
.PHONY : tools/player/CMakeFiles/playerlib.dir/src/PlayerModule.cpp.o.requires

tools/player/CMakeFiles/playerlib.dir/src/PlayerModule.cpp.o.provides: tools/player/CMakeFiles/playerlib.dir/src/PlayerModule.cpp.o.requires
	$(MAKE) -f tools/player/CMakeFiles/playerlib.dir/build.make tools/player/CMakeFiles/playerlib.dir/src/PlayerModule.cpp.o.provides.build
.PHONY : tools/player/CMakeFiles/playerlib.dir/src/PlayerModule.cpp.o.provides

tools/player/CMakeFiles/playerlib.dir/src/PlayerModule.cpp.o.provides.build: tools/player/CMakeFiles/playerlib.dir/src/PlayerModule.cpp.o

# Object files for target playerlib
playerlib_OBJECTS = \
"CMakeFiles/playerlib.dir/src/PlayerModule.cpp.o"

# External object files for target playerlib
playerlib_EXTERNAL_OBJECTS =

tools/player/libplayerlib.a: tools/player/CMakeFiles/playerlib.dir/src/PlayerModule.cpp.o
tools/player/libplayerlib.a: tools/player/CMakeFiles/playerlib.dir/build.make
tools/player/libplayerlib.a: tools/player/CMakeFiles/playerlib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX static library libplayerlib.a"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/player && $(CMAKE_COMMAND) -P CMakeFiles/playerlib.dir/cmake_clean_target.cmake
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/player && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/playerlib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tools/player/CMakeFiles/playerlib.dir/build: tools/player/libplayerlib.a
.PHONY : tools/player/CMakeFiles/playerlib.dir/build

tools/player/CMakeFiles/playerlib.dir/requires: tools/player/CMakeFiles/playerlib.dir/src/PlayerModule.cpp.o.requires
.PHONY : tools/player/CMakeFiles/playerlib.dir/requires

tools/player/CMakeFiles/playerlib.dir/clean:
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/player && $(CMAKE_COMMAND) -P CMakeFiles/playerlib.dir/cmake_clean.cmake
.PHONY : tools/player/CMakeFiles/playerlib.dir/clean

tools/player/CMakeFiles/playerlib.dir/depend:
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/tools/player /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/player /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/tools/player/CMakeFiles/playerlib.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tools/player/CMakeFiles/playerlib.dir/depend

