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
include hesperia/egocontroller/CMakeFiles/egocontroller.dir/depend.make

# Include the progress variables for this target.
include hesperia/egocontroller/CMakeFiles/egocontroller.dir/progress.make

# Include the compile flags for this target's objects.
include hesperia/egocontroller/CMakeFiles/egocontroller.dir/flags.make

hesperia/egocontroller/CMakeFiles/egocontroller.dir/apps/egocontroller.cpp.o: hesperia/egocontroller/CMakeFiles/egocontroller.dir/flags.make
hesperia/egocontroller/CMakeFiles/egocontroller.dir/apps/egocontroller.cpp.o: ../hesperia/egocontroller/apps/egocontroller.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object hesperia/egocontroller/CMakeFiles/egocontroller.dir/apps/egocontroller.cpp.o"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/hesperia/egocontroller && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/egocontroller.dir/apps/egocontroller.cpp.o -c /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/hesperia/egocontroller/apps/egocontroller.cpp

hesperia/egocontroller/CMakeFiles/egocontroller.dir/apps/egocontroller.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/egocontroller.dir/apps/egocontroller.cpp.i"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/hesperia/egocontroller && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/hesperia/egocontroller/apps/egocontroller.cpp > CMakeFiles/egocontroller.dir/apps/egocontroller.cpp.i

hesperia/egocontroller/CMakeFiles/egocontroller.dir/apps/egocontroller.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/egocontroller.dir/apps/egocontroller.cpp.s"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/hesperia/egocontroller && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/hesperia/egocontroller/apps/egocontroller.cpp -o CMakeFiles/egocontroller.dir/apps/egocontroller.cpp.s

hesperia/egocontroller/CMakeFiles/egocontroller.dir/apps/egocontroller.cpp.o.requires:
.PHONY : hesperia/egocontroller/CMakeFiles/egocontroller.dir/apps/egocontroller.cpp.o.requires

hesperia/egocontroller/CMakeFiles/egocontroller.dir/apps/egocontroller.cpp.o.provides: hesperia/egocontroller/CMakeFiles/egocontroller.dir/apps/egocontroller.cpp.o.requires
	$(MAKE) -f hesperia/egocontroller/CMakeFiles/egocontroller.dir/build.make hesperia/egocontroller/CMakeFiles/egocontroller.dir/apps/egocontroller.cpp.o.provides.build
.PHONY : hesperia/egocontroller/CMakeFiles/egocontroller.dir/apps/egocontroller.cpp.o.provides

hesperia/egocontroller/CMakeFiles/egocontroller.dir/apps/egocontroller.cpp.o.provides.build: hesperia/egocontroller/CMakeFiles/egocontroller.dir/apps/egocontroller.cpp.o

# Object files for target egocontroller
egocontroller_OBJECTS = \
"CMakeFiles/egocontroller.dir/apps/egocontroller.cpp.o"

# External object files for target egocontroller
egocontroller_EXTERNAL_OBJECTS =

hesperia/egocontroller/egocontroller: hesperia/egocontroller/CMakeFiles/egocontroller.dir/apps/egocontroller.cpp.o
hesperia/egocontroller/egocontroller: hesperia/egocontroller/CMakeFiles/egocontroller.dir/build.make
hesperia/egocontroller/egocontroller: hesperia/egocontroller/libegocontrollerlib.a
hesperia/egocontroller/egocontroller: hesperia/libhesperia/libhesperia.a
hesperia/egocontroller/egocontroller: libopendavinci/libopendavinci.a
hesperia/egocontroller/egocontroller: 3rdParty/libzip-0.9/libziplib.a
hesperia/egocontroller/egocontroller: 3rdParty/zlib-1.2.3/libzlib.a
hesperia/egocontroller/egocontroller: 3rdParty/libdb-4.6.21/libdblib.a
hesperia/egocontroller/egocontroller: hesperia/egocontroller/CMakeFiles/egocontroller.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable egocontroller"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/hesperia/egocontroller && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/egocontroller.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
hesperia/egocontroller/CMakeFiles/egocontroller.dir/build: hesperia/egocontroller/egocontroller
.PHONY : hesperia/egocontroller/CMakeFiles/egocontroller.dir/build

hesperia/egocontroller/CMakeFiles/egocontroller.dir/requires: hesperia/egocontroller/CMakeFiles/egocontroller.dir/apps/egocontroller.cpp.o.requires
.PHONY : hesperia/egocontroller/CMakeFiles/egocontroller.dir/requires

hesperia/egocontroller/CMakeFiles/egocontroller.dir/clean:
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/hesperia/egocontroller && $(CMAKE_COMMAND) -P CMakeFiles/egocontroller.dir/cmake_clean.cmake
.PHONY : hesperia/egocontroller/CMakeFiles/egocontroller.dir/clean

hesperia/egocontroller/CMakeFiles/egocontroller.dir/depend:
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/hesperia/egocontroller /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/hesperia/egocontroller /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/hesperia/egocontroller/CMakeFiles/egocontroller.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : hesperia/egocontroller/CMakeFiles/egocontroller.dir/depend

