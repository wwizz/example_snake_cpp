cmake_minimum_required(VERSION 3.6)

include(ExternalProjectUtils)

set_external_library(mbedcrypto)

if(NOT EXISTS ${LIBRARY_PATH})
    set_library_path(mbedcrypto MBEDCRYPTO_PATH "")
    set_library_path(mbedx509   MBEDX509_PATH "")
    set_library_path(mbedtls    MBEDTLS_PATH "")

    # The patch for mbedTLS defines the mbedtls configuration header file. I
    # tried including mbedtls-config.h through CMAKE_ARGS but that did not
    # work because CMake tried to use it during the identification of the
    # cross-compilers.

    if (${CMAKE_PROJECT_NAME} MATCHES "libhuestream")
        set(CUSTOM_CONFIG_C_FLAGS "-DMBEDTLS_CONFIG_FILE=\\\"${CMAKE_CURRENT_LIST_DIR}/patches/mbedtls-config.h\\\"")
    endif()

    set(EXTERNAL_DEPENDENCY external_mbedtls)

    ExternalProject_Add(${EXTERNAL_DEPENDENCY}
        PREFIX ${EXTERNAL_LIBRARIES_BUILD_PATH}
        DOWNLOAD_COMMAND ${CMAKE_COMMAND} -DREFERENCE=${GIT_REFERENCE} -DREPO=${mbedtls_URL} -DBRANCH=${mbedtls_VERSION} -DSOURCE_DIR=${EXTERNAL_LIBRARIES_SOURCE_PATH}/mbedtls -DPATCH=mbedtls.patch -P ${CMAKE_CURRENT_LIST_DIR}/CloneRepository.cmake
        UPDATE_COMMAND ""
        SOURCE_DIR "${EXTERNAL_LIBRARIES_SOURCE_PATH}/mbedtls"
        CMAKE_ARGS ${COMMON_ARGS} -DUSE_STATIC_MBEDTLS_LIBRARY=ON -DUSE_SHARED_MBEDTLS_LIBRARY=OFF -DENABLE_TESTING=OFF -DENABLE_PROGRAMS=OFF
        BUILD_BYPRODUCTS ${MBEDCRYPTO_PATH} ${MBEDX509_PATH} ${MBEDTLS_PATH}
        LIST_SEPARATOR ^^
    )
endif(NOT EXISTS ${LIBRARY_PATH})

expose_external_library()
expose_additional_external_library(mbedx509 mbedcrypto)
expose_additional_external_library(mbedtls mbedx509)

if(WIN32)
    set_property(TARGET mbedtls APPEND PROPERTY INTERFACE_LINK_LIBRARIES iphlpapi ws2_32)
endif(WIN32)