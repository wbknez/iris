# Include unit testing library.
include(ExternalDependencies)

# Test files.
file(GLOB Iris_TEST_FILES
  ${Iris_SOURCE_DIR}/test/*.cpp
  ${Iris_SOURCE_DIR}/test/iris/*.cpp
  ${Iris_SOURCE_DIR}/test/iris/gen/*.cpp
  ${Iris_SOURCE_DIR}/test/iris/io/*.cpp
  ${Iris_SOURCE_DIR}/test/iris/io/reader/*.cpp
  ${Iris_SOURCE_DIR}/test/iris/io/writer/*.cpp)

include_directories(${EXT_PROJECTS_DIR}/catch/include ${Iris_SOURCE_DIR}/include)
add_executable(iris_test ${Iris_TEST_FILES})
target_link_libraries(iris_test Catch IrisLib Threads::Threads)
