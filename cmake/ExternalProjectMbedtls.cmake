###############################################################################
# Mbed tls
###############################################################################
set(CUSTOM_CONFIG_C_FLAGS "-DMBEDTLS_CONFIG_FILE=\\\"custom_config.h\\\" -I${CMAKE_CURRENT_LIST_DIR}")

ExternalProject_Add(
        Mbedtls
        GIT_REPOSITORY "https://github.com/ARMmbed/mbedtls.git"
        GIT_TAG "mbedtls-2.3.0"

        PATCH_COMMAND ${CMAKE_COMMAND} -DPATCH=${CMAKE_SOURCE_DIR}/PATCHES/mbedtls_fix_dynamic_library_dependencies.diff -P ${CMAKE_SOURCE_DIR}/CMAKE/Patch.cmake
        UPDATE_COMMAND ""

        PREFIX "${CMAKE_BINARY_DIR}/mbedtls"
        CMAKE_ARGS -DCMAKE_C_FLAGS=${CUSTOM_CONFIG_C_FLAGS}  -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR} -DENABLE_TESTING=OFF -DUSE_SHARED_MBEDTLS_LIBRARY=ON -DUSE_STATIC_MBEDTLS_LIBRARY=ON -DENABLE_PROGRAMS=OFF
        TEST_COMMAND ""
)


set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${CMAKE_BINARY_DIR}/mbedtls")
find_and_copy_libs(Mbedtls "mbedtls;mbedcrypto;mbedx509" "${CMAKE_BINARY_DIR}/bin")
