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
include examples/example5/example5sender/CMakeFiles/example5sender.dir/depend.make

# Include the progress variables for this target.
include examples/example5/example5sender/CMakeFiles/example5sender.dir/progress.make

# Include the compile flags for this target's objects.
include examples/example5/example5sender/CMakeFiles/example5sender.dir/flags.make

examples/example5/example5sender/CMakeFiles/example5sender.dir/MainModule.cpp.o: examples/example5/example5sender/CMakeFiles/example5sender.dir/flags.make
examples/example5/example5sender/CMakeFiles/example5sender.dir/MainModule.cpp.o: ../examples/example5/example5sender/MainModule.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object examples/example5/example5sender/CMakeFiles/example5sender.dir/MainModule.cpp.o"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/examples/example5/example5sender && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/example5sender.dir/MainModule.cpp.o -c /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/examples/example5/example5sender/MainModule.cpp

examples/example5/example5sender/CMakeFiles/example5sender.dir/MainModule.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/example5sender.dir/MainModule.cpp.i"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/examples/example5/example5sender && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/examples/example5/example5sender/MainModule.cpp > CMakeFiles/example5sender.dir/MainModule.cpp.i

examples/example5/example5sender/CMakeFiles/example5sender.dir/MainModule.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/example5sender.dir/MainModule.cpp.s"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/examples/example5/example5sender && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/examples/example5/example5sender/MainModule.cpp -o CMakeFiles/example5sender.dir/MainModule.cpp.s

examples/example5/example5sender/CMakeFiles/example5sender.dir/MainModule.cpp.o.requires:
.PHONY : examples/example5/example5sender/CMakeFiles/example5sender.dir/MainModule.cpp.o.requires

examples/example5/example5sender/CMakeFiles/example5sender.dir/MainModule.cpp.o.provides: examples/example5/example5sender/CMakeFiles/example5sender.dir/MainModule.cpp.o.requires
	$(MAKE) -f examples/example5/example5sender/CMakeFiles/example5sender.dir/build.make examples/example5/example5sender/CMakeFiles/example5sender.dir/MainModule.cpp.o.provides.build
.PHONY : examples/example5/example5sender/CMakeFiles/example5sender.dir/MainModule.cpp.o.provides

examples/example5/example5sender/CMakeFiles/example5sender.dir/MainModule.cpp.o.provides.build: examples/example5/example5sender/CMakeFiles/example5sender.dir/MainModule.cpp.o

examples/example5/example5sender/CMakeFiles/example5sender.dir/Example5Sender.cpp.o: examples/example5/example5sender/CMakeFiles/example5sender.dir/flags.make
examples/example5/example5sender/CMakeFiles/example5sender.dir/Example5Sender.cpp.o: ../examples/example5/example5sender/Example5Sender.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object examples/example5/example5sender/CMakeFiles/example5sender.dir/Example5Sender.cpp.o"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/examples/example5/example5sender && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/example5sender.dir/Example5Sender.cpp.o -c /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/examples/example5/example5sender/Example5Sender.cpp

examples/example5/example5sender/CMakeFiles/example5sender.dir/Example5Sender.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/example5sender.dir/Example5Sender.cpp.i"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/examples/example5/example5sender && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/examples/example5/example5sender/Example5Sender.cpp > CMakeFiles/example5sender.dir/Example5Sender.cpp.i

examples/example5/example5sender/CMakeFiles/example5sender.dir/Example5Sender.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/example5sender.dir/Example5Sender.cpp.s"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/examples/example5/example5sender && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/examples/example5/example5sender/Example5Sender.cpp -o CMakeFiles/example5sender.dir/Example5Sender.cpp.s

examples/example5/example5sender/CMakeFiles/example5sender.dir/Example5Sender.cpp.o.requires:
.PHONY : examples/example5/example5sender/CMakeFiles/example5sender.dir/Example5Sender.cpp.o.requires

examples/example5/example5sender/CMakeFiles/example5sender.dir/Example5Sender.cpp.o.provides: examples/example5/example5sender/CMakeFiles/example5sender.dir/Example5Sender.cpp.o.requires
	$(MAKE) -f examples/example5/example5sender/CMakeFiles/example5sender.dir/build.make examples/example5/example5sender/CMakeFiles/example5sender.dir/Example5Sender.cpp.o.provides.build
.PHONY : examples/example5/example5sender/CMakeFiles/example5sender.dir/Example5Sender.cpp.o.provides

examples/example5/example5sender/CMakeFiles/example5sender.dir/Example5Sender.cpp.o.provides.build: examples/example5/example5sender/CMakeFiles/example5sender.dir/Example5Sender.cpp.o

# Object files for target example5sender
example5sender_OBJECTS = \
"CMakeFiles/example5sender.dir/MainModule.cpp.o" \
"CMakeFiles/example5sender.dir/Example5Sender.cpp.o"

# External object files for target example5sender
example5sender_EXTERNAL_OBJECTS =

examples/example5/example5sender/example5sender: examples/example5/example5sender/CMakeFiles/example5sender.dir/MainModule.cpp.o
examples/example5/example5sender/example5sender: examples/example5/example5sender/CMakeFiles/example5sender.dir/Example5Sender.cpp.o
examples/example5/example5sender/example5sender: examples/example5/example5sender/CMakeFiles/example5sender.dir/build.make
examples/example5/example5sender/example5sender: libopendavinci/libopendavinci.a
examples/example5/example5sender/example5sender: 3rdParty/libzip-0.9/libziplib.a
examples/example5/example5sender/example5sender: 3rdParty/zlib-1.2.3/libzlib.a
examples/example5/example5sender/example5sender: 3rdParty/libdb-4.6.21/libdblib.a
examples/example5/example5sender/example5sender: examples/example5/example5sender/CMakeFiles/example5sender.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable example5sender"
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/examples/example5/example5sender && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/example5sender.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/example5/example5sender/CMakeFiles/example5sender.dir/build: examples/example5/example5sender/example5sender
.PHONY : examples/example5/example5sender/CMakeFiles/example5sender.dir/build

examples/example5/example5sender/CMakeFiles/example5sender.dir/requires: examples/example5/example5sender/CMakeFiles/example5sender.dir/MainModule.cpp.o.requires
examples/example5/example5sender/CMakeFiles/example5sender.dir/requires: examples/example5/example5sender/CMakeFiles/example5sender.dir/Example5Sender.cpp.o.requires
.PHONY : examples/example5/example5sender/CMakeFiles/example5sender.dir/requires

examples/example5/example5sender/CMakeFiles/example5sender.dir/clean:
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/examples/example5/example5sender && $(CMAKE_COMMAND) -P CMakeFiles/example5sender.dir/cmake_clean.cmake
.PHONY : examples/example5/example5sender/CMakeFiles/example5sender.dir/clean

examples/example5/example5sender/CMakeFiles/example5sender.dir/depend:
	cd /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/examples/example5/example5sender /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/examples/example5/example5sender /home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/examples/example5/example5sender/CMakeFiles/example5sender.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/example5/example5sender/CMakeFiles/example5sender.dir/depend

