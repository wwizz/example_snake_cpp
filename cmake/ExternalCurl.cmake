cmake_minimum_required(VERSION 3.6)

include(ExternalProjectUtils)

set_external_library(curl mbedtls)

# curl is special and uses the lib prefix on windows
if (MSVC)
    set(LIBRARY_FILE_PATH libcurl.lib)
	set(LIBRARY_PATH ${EXTERNAL_LIBRARIES_INSTALL_PATH}/lib/libcurl.lib)
    set_library_path(${LIBRARY_NAME} CURL_PATH libcurl.lib)
else()
    set_library_path(${LIBRARY_NAME} CURL_PATH "")
endif()

if (NOT EXISTS ${LIBRARY_PATH})
    # The patch for cURL:
    # - Adds a variable that specifies the path to mbedTLS (CURL_WITH_MBEDTLS)
    # - Feature-tests strerror_r only by compiling and not by running. This allows
    #   us to determine which version of strerror_r is present while cross-
    #   compiling.
    set(EXTERNAL_DEPENDENCY external_curl)

    ExternalProject_Add(${EXTERNAL_DEPENDENCY}
            PREFIX ${EXTERNAL_LIBRARIES_BUILD_PATH}
            DOWNLOAD_COMMAND ${CMAKE_COMMAND} -DREFERENCE=${GIT_REFERENCE} -DREPO=https://github.com/curl/curl.git -DBRANCH=curl-7_53_1 -DSOURCE_DIR=${EXTERNAL_LIBRARIES_SOURCE_PATH}/curl -DPATCH=curl.patch -P ${CMAKE_CURRENT_LIST_DIR}/CloneRepository.cmake
            UPDATE_COMMAND ""
            SOURCE_DIR "${EXTERNAL_LIBRARIES_SOURCE_PATH}/curl"
            CMAKE_ARGS ${COMMON_ARGS} -DINCLUDE_DIRS=${EXTERNAL_LIBRARIES_INSTALL_PATH}/include -DCURL_WITH_MBEDTLS=${EXTERNAL_LIBRARIES_INSTALL_PATH} -DUSE_MBEDTLS=ON -DCMAKE_USE_OPENSSL=OFF -DSSL_ENABLED=ON -DHTTP_ONLY=ON -DCURL_STATICLIB=ON -DBUILD_CURL_EXE=OFF -DBUILD_TESTING=OFF -DCURL_DISABLE_LDAP=ON -DCMAKE_USE_LIBSSH2=OFF -DCURL_ZLIB=OFF -DCURL_WINDOWS_SSPI=OFF
            DEPENDS mbedtls
            BUILD_BYPRODUCTS ${CURL_PATH}
            LIST_SEPARATOR ^^
            )
endif()

expose_external_library()

set_property(TARGET ${LIBRARY_NAME} APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS CURL_STATICLIB)