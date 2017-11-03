cmake_minimum_required(VERSION 3.6)

include(Common)

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

#
# support sources
#
file(GLOB_RECURSE SUPPORT_SOURCES
        "chrono/*.cpp" "chrono/*.h"
        "std/*.cpp" "std/*.h"
        "date/*.cpp" "date/*.h"
        "chrono/*.cpp" "chrono/*.h"
        "logging/*.cpp" "logging/*.h"
        "threading/*.cpp" "threading/*.h"
        "network/*.cpp" "network/*.h"
        "util/*.cpp" "util/*.h"
)

#
# sources for support-test
#

file(GLOB_RECURSE SUPPORT_TEST_SOURCES
        "logging/_test/*"
        "network/_test/*"
        "network/http/_test/*"
        "network/http/curl/_test/*"
        "network/sockets/_test/*"
        "network/sockets/bsd/_test/*"
)

list(REMOVE_ITEM SUPPORT_SOURCES ${SUPPORT_TEST_SOURCES})

#
# set client-specific sources, definitions, and compiler flags
#

file(GLOB_RECURSE NETWORK_HTTP_SOURCES_CURL "network/http/curl/*.cpp")
file(GLOB_RECURSE NETWORK_HTTP_SOURCES_OBJC "network/http/objc/*.mm")
# remove http client sources from support-sources
list(REMOVE_ITEM SUPPORT_SOURCES ${NETWORK_HTTP_SOURCES_OBJC} ${NETWORK_HTTP_SOURCES_CURL})

string(TOUPPER ${HTTP_CLIENT} HTTP_CLIENT_ID)
list(APPEND SUPPORT_DEFINITIONS_PUBLIC "HTTP_CLIENT_${HTTP_CLIENT_ID}")

if(HTTP_CLIENT STREQUAL "objc")
    list(APPEND SUPPORT_DEFINITIONS_PRIVATE "DEFAULT_HTTP_CLIENT=ObjcHttpClient")
    list(APPEND SUPPORT_DEFINITIONS_PRIVATE "DEFAULT_HTTP_CLIENT_INCLUDE=network/http/objc/ObjcHttpClient.h")

    set_source_files_properties(${NETWORK_HTTP_SOURCES_OBJC} PROPERTY COMPILE_FLAGS "-Wno-deprecated-declarations")
    list(APPEND SUPPORT_SOURCES ${NETWORK_HTTP_SOURCES_OBJC})
elseif(HTTP_CLIENT STREQUAL "curl")
    list(APPEND SUPPORT_DEFINITIONS_PRIVATE "DEFAULT_HTTP_CLIENT=CurlHttpClient")
    list(APPEND SUPPORT_DEFINITIONS_PRIVATE "DEFAULT_HTTP_CLIENT_INCLUDE=network/http/curl/CurlHttpClient.h")

    list(APPEND SUPPORT_SOURCES ${NETWORK_HTTP_SOURCES_CURL})
    list(APPEND SUPPORT_LINK_LIBRARIES_PUBLIC curl)

    # the default cabundle only contains the verisign cert used by meethue
    # you can generate the a complete cabundle.txt by running the create_bundle.py script
    if(NOT CURL_CABUNDLE_FILE)
        set(CURL_CABUNDLE_FILE CABundle.txt)
    endif(NOT CURL_CABUNDLE_FILE)
    list(APPEND SUPPORT_DEFINITIONS_PRIVATE "CURL_CABUNDLE_FILE=${CURL_CABUNDLE_FILE}")
elseif(HTTP_CLIENT STREQUAL "none")
    # no default, the user must call HttpRequest::set_http_client() at runtime
endif()

if(NOT CERTIFICATE_MAP_FILE)
    set(CERTIFICATE_MAP_FILE DefaultCertificates.txt)
endif()
list(APPEND SUPPORT_DEFINITIONS_PRIVATE "CERTIFICATE_MAP_FILE=${CERTIFICATE_MAP_FILE}")

# enable JNI if that was explicitly required from outside
if(ENABLE_JNI)
    if(NOT ANDROID)
        find_package(JNI REQUIRED)
        list(APPEND SUPPORT_PRIVATE_INCLUDES ${JNI_INCLUDE_DIRS})
    endif(NOT ANDROID)

    # add jni sources
    file(GLOB_RECURSE SUPPORT_SOURCES_JNI "jni/*.cpp" "jni/*.h")
    list(APPEND SUPPORT_SOURCES ${SUPPORT_SOURCES_JNI})
endif(ENABLE_JNI)

# set additional libraries
if (NOT WIN32)
    list(APPEND SUPPORT_LINK_LIBRARIES_PUBLIC dl)
else()
    list(APPEND SUPPORT_LINK_LIBRARIES_PUBLIC iphlpapi ws2_32)
endif()

if(APPLE)
    find_library(FOUNDATION Foundation)
    find_library(CFNETWORK CFNetwork)
    find_library(SECURITY Security)
    list(APPEND SUPPORT_LINK_LIBRARIES_PUBLIC ${FOUNDATION} ${CFNETWORK} ${SECURITY})

    if(LOG_WITH_NSLOG)
        set_source_files_properties(logging/LoggerConsole.cpp PROPERTY COMPILE_FLAGS "-x objective-c++ -DLOG_WITH_NSLOG")
    endif(LOG_WITH_NSLOG)
endif()

find_package(Threads REQUIRED)

if(ANDROID)
    list(APPEND SUPPORT_DEFINITIONS_PUBLIC ANDROID ANDROID_LOGCAT)
endif()

if(HTTPS_PROXY_DISABLED)
    list(APPEND SUPPORT_DEFINITIONS_PUBLIC "HTTPS_PROXY_DISABLED=1")
endif(HTTPS_PROXY_DISABLED)

if(VERBOSE_HTTP_LOGGING)
    list(APPEND SUPPORT_DEFINITIONS_PUBLIC VERBOSE_HTTP_LOGGING)
endif(VERBOSE_HTTP_LOGGING)

#
# create support libraries
#

function(add_support_library NAME)
    add_library(${NAME} STATIC ${SUPPORT_SOURCES})
    target_link_libraries(${NAME} PUBLIC ${SUPPORT_LINK_LIBRARIES_PUBLIC})
    target_compile_definitions(${NAME} PRIVATE ${SUPPORT_DEFINITIONS_PRIVATE})
    target_compile_definitions(${NAME} PUBLIC ${SUPPORT_DEFINITIONS_PUBLIC})
    # add the root of this support repo to the include path
    target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_include_directories(${NAME} PRIVATE ${SUPPORT_PRIVATE_INCLUDES})
    set_target_properties(${NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
    treat_warning_as_error(${NAME})
endfunction()

add_support_library(support)

if(EXPOSE_TEST_TARGETS)
    list(APPEND SUPPORT_SOURCES ${SUPPORT_TEST_SOURCES})
    list(APPEND SUPPORT_DEFINITIONS_PUBLIC SDK_TEST)
    add_support_library(support_test)
endif(EXPOSE_TEST_TARGETS)