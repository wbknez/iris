# Header files.
include_directories(${Iris_SOURCE_DIR}/include ${LIBUUID_INCLUDE_DIRS})

# Source files.
file(GLOB Iris_SOURCE_FILES
  ${Iris_SOURCE_DIR}/src/iris/*.cpp
  ${Iris_SOURCE_DIR}/src/iris/gen/*.cpp
  ${Iris_SOURCE_DIR}/src/iris/io/*.cpp
  ${Iris_SOURCE_DIR}/src/iris/io/reader/*.cpp
  ${Iris_SOURCE_DIR}/src/iris/io/writer/*.cpp)

# Build library.
add_library(IrisLib ${Iris_SOURCE_FILES})

# Build executable.
add_executable(iris ${Iris_SOURCE_DIR}/src/main.cpp)
target_link_libraries(iris IrisLib Threads::Threads)
