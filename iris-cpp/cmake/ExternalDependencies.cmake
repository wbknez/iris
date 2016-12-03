# -*- mode: CMake -*-
include(ExternalProject)

# Require git.
find_package(Git REQUIRED)

# Set the location where external libraries should go/be built.
set(EXT_PROJECTS_DIR ${CMAKE_BINARY_DIR}/external)

# Catch
#
# For unit testing.
ExternalProject_add(Catch-External
    PREFIX ${EXT_PROJECTS_DIR}/catch
    GIT_REPOSITORY https://github.com/philsquared/Catch.git
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/external/catch/src/Catch-External/single_include/catch.hpp ${CMAKE_BINARY_DIR}/external/catch/include/catch.hpp
    LOG_DOWNLOAD ON
    )

# Build Catch as a "library".
add_library(Catch INTERFACE)
add_dependencies(Catch Catch-External)
target_include_directories(Catch INTERFACE ${CMAKE_BINARY_DIR}/external/catch/include)
