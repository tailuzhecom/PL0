# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /home/young/下载/clion-2018.1.4/bin/cmake/bin/cmake

# The command to remove a file.
RM = /home/young/下载/clion-2018.1.4/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/young/CLionProjects/PL/0

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/young/CLionProjects/PL/0/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/0.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/0.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/0.dir/flags.make

CMakeFiles/0.dir/main.cpp.o: CMakeFiles/0.dir/flags.make
CMakeFiles/0.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/young/CLionProjects/PL/0/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/0.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/0.dir/main.cpp.o -c /home/young/CLionProjects/PL/0/main.cpp

CMakeFiles/0.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/0.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/young/CLionProjects/PL/0/main.cpp > CMakeFiles/0.dir/main.cpp.i

CMakeFiles/0.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/0.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/young/CLionProjects/PL/0/main.cpp -o CMakeFiles/0.dir/main.cpp.s

CMakeFiles/0.dir/main.cpp.o.requires:

.PHONY : CMakeFiles/0.dir/main.cpp.o.requires

CMakeFiles/0.dir/main.cpp.o.provides: CMakeFiles/0.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/0.dir/build.make CMakeFiles/0.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/0.dir/main.cpp.o.provides

CMakeFiles/0.dir/main.cpp.o.provides.build: CMakeFiles/0.dir/main.cpp.o


CMakeFiles/0.dir/Parser.cpp.o: CMakeFiles/0.dir/flags.make
CMakeFiles/0.dir/Parser.cpp.o: ../Parser.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/young/CLionProjects/PL/0/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/0.dir/Parser.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/0.dir/Parser.cpp.o -c /home/young/CLionProjects/PL/0/Parser.cpp

CMakeFiles/0.dir/Parser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/0.dir/Parser.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/young/CLionProjects/PL/0/Parser.cpp > CMakeFiles/0.dir/Parser.cpp.i

CMakeFiles/0.dir/Parser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/0.dir/Parser.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/young/CLionProjects/PL/0/Parser.cpp -o CMakeFiles/0.dir/Parser.cpp.s

CMakeFiles/0.dir/Parser.cpp.o.requires:

.PHONY : CMakeFiles/0.dir/Parser.cpp.o.requires

CMakeFiles/0.dir/Parser.cpp.o.provides: CMakeFiles/0.dir/Parser.cpp.o.requires
	$(MAKE) -f CMakeFiles/0.dir/build.make CMakeFiles/0.dir/Parser.cpp.o.provides.build
.PHONY : CMakeFiles/0.dir/Parser.cpp.o.provides

CMakeFiles/0.dir/Parser.cpp.o.provides.build: CMakeFiles/0.dir/Parser.cpp.o


# Object files for target 0
0_OBJECTS = \
"CMakeFiles/0.dir/main.cpp.o" \
"CMakeFiles/0.dir/Parser.cpp.o"

# External object files for target 0
0_EXTERNAL_OBJECTS =

0 : CMakeFiles/0.dir/main.cpp.o
0 : CMakeFiles/0.dir/Parser.cpp.o
0 : CMakeFiles/0.dir/build.make
0 : CMakeFiles/0.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/young/CLionProjects/PL/0/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable 0"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/0.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/0.dir/build: 0

.PHONY : CMakeFiles/0.dir/build

CMakeFiles/0.dir/requires: CMakeFiles/0.dir/main.cpp.o.requires
CMakeFiles/0.dir/requires: CMakeFiles/0.dir/Parser.cpp.o.requires

.PHONY : CMakeFiles/0.dir/requires

CMakeFiles/0.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/0.dir/cmake_clean.cmake
.PHONY : CMakeFiles/0.dir/clean

CMakeFiles/0.dir/depend:
	cd /home/young/CLionProjects/PL/0/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/young/CLionProjects/PL/0 /home/young/CLionProjects/PL/0 /home/young/CLionProjects/PL/0/cmake-build-debug /home/young/CLionProjects/PL/0/cmake-build-debug /home/young/CLionProjects/PL/0/cmake-build-debug/CMakeFiles/0.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/0.dir/depend

