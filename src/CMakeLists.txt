set(CMAKE_CXX_STANDARD 11)

include_directories(../edk/libhuestream)
include_directories(.)

file(GLOB_RECURSE SRC_FILES *.cpp)
file(GLOB_RECURSE HDR_FILES *.h)

add_executable(example_snake_cpp ${SRC_FILES} ${HDR_FILES})
add_dependencies(example_snake_cpp huestream)