# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/caro/Documents/RN_22_23/praxis1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/caro/Documents/RN_22_23/praxis1/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/helper_functions.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/helper_functions.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/helper_functions.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/helper_functions.dir/flags.make

CMakeFiles/helper_functions.dir/helper_functions.c.o: CMakeFiles/helper_functions.dir/flags.make
CMakeFiles/helper_functions.dir/helper_functions.c.o: ../helper_functions.c
CMakeFiles/helper_functions.dir/helper_functions.c.o: CMakeFiles/helper_functions.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/caro/Documents/RN_22_23/praxis1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/helper_functions.dir/helper_functions.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/helper_functions.dir/helper_functions.c.o -MF CMakeFiles/helper_functions.dir/helper_functions.c.o.d -o CMakeFiles/helper_functions.dir/helper_functions.c.o -c /home/caro/Documents/RN_22_23/praxis1/helper_functions.c

CMakeFiles/helper_functions.dir/helper_functions.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/helper_functions.dir/helper_functions.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/caro/Documents/RN_22_23/praxis1/helper_functions.c > CMakeFiles/helper_functions.dir/helper_functions.c.i

CMakeFiles/helper_functions.dir/helper_functions.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/helper_functions.dir/helper_functions.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/caro/Documents/RN_22_23/praxis1/helper_functions.c -o CMakeFiles/helper_functions.dir/helper_functions.c.s

# Object files for target helper_functions
helper_functions_OBJECTS = \
"CMakeFiles/helper_functions.dir/helper_functions.c.o"

# External object files for target helper_functions
helper_functions_EXTERNAL_OBJECTS =

libhelper_functions.a: CMakeFiles/helper_functions.dir/helper_functions.c.o
libhelper_functions.a: CMakeFiles/helper_functions.dir/build.make
libhelper_functions.a: CMakeFiles/helper_functions.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/caro/Documents/RN_22_23/praxis1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libhelper_functions.a"
	$(CMAKE_COMMAND) -P CMakeFiles/helper_functions.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/helper_functions.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/helper_functions.dir/build: libhelper_functions.a
.PHONY : CMakeFiles/helper_functions.dir/build

CMakeFiles/helper_functions.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/helper_functions.dir/cmake_clean.cmake
.PHONY : CMakeFiles/helper_functions.dir/clean

CMakeFiles/helper_functions.dir/depend:
	cd /home/caro/Documents/RN_22_23/praxis1/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/caro/Documents/RN_22_23/praxis1 /home/caro/Documents/RN_22_23/praxis1 /home/caro/Documents/RN_22_23/praxis1/cmake-build-debug /home/caro/Documents/RN_22_23/praxis1/cmake-build-debug /home/caro/Documents/RN_22_23/praxis1/cmake-build-debug/CMakeFiles/helper_functions.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/helper_functions.dir/depend

