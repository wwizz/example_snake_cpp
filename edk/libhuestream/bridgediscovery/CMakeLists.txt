cmake_minimum_required(VERSION 3.6)

file(GLOB_RECURSE BRIDGE_DISCOVERY_SOURCES "include/*.h" "src/*.h" "src/*.cpp")

add_library(bridge_discovery STATIC ${BRIDGE_DISCOVERY_SOURCES})
set_target_properties(bridge_discovery PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(bridge_discovery PUBLIC "./include/")
target_include_directories(bridge_discovery PRIVATE "./src/")
target_link_libraries(bridge_discovery PUBLIC support json)
treat_warning_as_error(bridge_discovery)
