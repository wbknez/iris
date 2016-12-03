cmake_minimum_required(VERSION 3.5)
project(Iris CXX)

set(CMAKE_MODULE_PATH ${Iris_SOURCE_DIR}/cmake)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

# C++11 threading requires pthreads.
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Squash non-unique random number error(s).
add_definitions(-DIRIS_WARN_ON_NONUNIQUE_RANDOM)

# Add debugging support if necessary.
if(CMAKE_BUILD_TYPE MATCHES Debug)
  add_definitions(-DIRIS_DEBUG)
endif()

# The project has the following directory structure:
#  - include
#  - src
#  - test
# containing the header files, source files, and test files, respectively.
#
# Add src as a managed sub-project.
add_subdirectory(src)
# And test(s).
add_subdirectory(test)

# Enable unit testing.
enable_testing(true)

# Build tests.
add_test(NAME TestBase COMMAND iris_test)
