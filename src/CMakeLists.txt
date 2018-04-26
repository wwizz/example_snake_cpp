set(CMAKE_CXX_STANDARD 11)

include_directories(../edk/libhuestream)
include_directories(../edk/libhuestream/support/include)
include_directories(../edk/libhuestream/bridgediscovery/include)
include_directories(../edk/3rd_party/edtls/libedtls)
include_directories(../edk/3rd_party/boost/src)
include_directories(../edk/3rd_party)
include_directories(.)

file(GLOB_RECURSE SRC_FILES *.cpp)
file(GLOB_RECURSE HDR_FILES *.h)

add_executable(example_snake_cpp ${SRC_FILES} ${HDR_FILES})
target_link_libraries(example_snake_cpp huestream)