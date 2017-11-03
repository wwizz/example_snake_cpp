include_directories(
        ..
)

file(GLOB_RECURSE huestream_files "*.cpp" "*.h")

add_library(huestream ${LIB_BUILD_MODE} ${huestream_files})
target_include_directories(huestream INTERFACE "..")

set(LIBS bridge_discovery support json edtls_client mbedcl_wrapper mbedtls mbedcrypto mbedx509)

if (WIN32)
    set(LIBS ${LIBS} Iphlpapi ws2_32)
endif()

if (ANDROID)
    set(LIBS ${LIBS} atomic)
endif()

add_library_dependencies(huestream ${LIBS})

install(TARGETS huestream
        RUNTIME DESTINATION ${INSTALL_DIR}
        LIBRARY DESTINATION ${INSTALL_DIR}
        ARCHIVE DESTINATION ${INSTALL_DIR})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/libhuestream/huestream DESTINATION ${INSTALL_DIR}/include
        FILES_MATCHING PATTERN "*.h")
