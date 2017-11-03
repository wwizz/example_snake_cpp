cmake_minimum_required(VERSION 3.6)

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

file(GLOB_RECURSE BRIDGE_DISCOVERY_SOURCES "*.cpp" "*.h")

file(GLOB_RECURSE BRIDGE_DISCOVERY_TEST_HELPER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/bridgediscovery/method/_test/*")

# create bridge discovery library
#
add_library(bridge_discovery STATIC ${BRIDGE_DISCOVERY_SOURCES} )
target_link_libraries(bridge_discovery PUBLIC support json)
set_target_properties(bridge_discovery PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(bridge_discovery PUBLIC ".")
treat_warning_as_error(bridge_discovery)

# create bridge discovery library with testing support
#
if(EXPOSE_TEST_TARGETS)
    add_library(bridge_discovery_test STATIC ${BRIDGE_DISCOVERY_SOURCES} ${BRIDGE_DISCOVERY_TEST_HELPER_SOURCES})
    target_link_libraries(bridge_discovery_test PUBLIC support_test json)
    set_target_properties(bridge_discovery_test PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_include_directories(bridge_discovery_test PUBLIC ".")
    treat_warning_as_error(bridge_discovery_test)
endif(EXPOSE_TEST_TARGETS)
