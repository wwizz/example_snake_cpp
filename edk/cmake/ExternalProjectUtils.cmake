cmake_minimum_required(VERSION 3.6)

include(ExternalProject)

# CMake unfortunately does *NOT* forward all variables when configuring an
# external project, most notably cross-compilation variables (TOOLCHAIN etc)
# which are used for Android and iOS builds.

if(ANDROID)
    set(COMMON_ARGS -DANDROID_DEPRECATED_HEADERS=ON -DANDROID_STL=c++_static -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM} -DANDROID_NDK=${ANDROID_NDK} -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} -DANDROID_NATIVE_API_LEVEL=${ANDROID_NATIVE_API_LEVEL} -DANDROID_ABI=${ANDROID_ABI})
endif()

if(APPLE)
    # cmake's externalproject_add cannot handle semicolons in the architecture list (e.g. "arm64;armv7")
    # so we need to replace them with ^^ and use the LIST_SEPARATOR parameter in ExternalProject_Add

    string(REPLACE ";" "^^" APPLE_ARCHS "${CMAKE_OSX_ARCHITECTURES}")

    set(COMMON_ARGS -DCMAKE_OSX_ARCHITECTURES=${APPLE_ARCHS} -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT})

    # to avoid problems with CHECK_TYPE_SIZE we need to select a single architecture when compiling for iphone

    if(APPLE_PLATFORM STREQUAL "iphoneos")
        set(COMMON_ARGS ${COMMON_ARGS} -DCMAKE_TRY_COMPILE_OSX_ARCHITECTURES=arm64)
    elseif(APPLE_PLATFORM STREQUAL "iphonesimulator")
        set(COMMON_ARGS ${COMMON_ARGS} -DCMAKE_TRY_COMPILE_OSX_ARCHITECTURES=x86_64)
    endif()

    set(APPLE_FLAGS ${DEPLOYMENT_TARGET})

    # cmake's externalproject_add cannot handle spaces in arguments, they need to be escaped

    if(ENABLE_LTO)
        set(APPLE_FLAGS "${APPLE_FLAGS}\ -flto")
    endif()

    if(ENABLE_EMBED_BITCODE)
        set(APPLE_FLAGS "${APPLE_FLAGS}\ -fembed-bitcode")
    endif()

    if(EXTERNALPROJECT_C_FLAGS)
        set(EXTERNALPROJECT_C_FLAGS ${APPLE_FLAGS}\ ${EXTERNALPROJECT_C_FLAGS})
    else()
        set(EXTERNALPROJECT_C_FLAGS ${APPLE_FLAGS})
    endif()

    if(EXTERNALPROJECT_CXX_FLAGS)
        set(EXTERNALPROJECT_CXX_FLAGS ${APPLE_FLAGS}\ ${EXTERNALPROJECT_CXX_FLAGS})
    else()
        set(EXTERNALPROJECT_CXX_FLAGS ${APPLE_FLAGS})
    endif()
endif()

set(COMMON_ARGS ${COMMON_ARGS} -DCMAKE_INSTALL_PREFIX=${EXTERNAL_LIBRARIES_INSTALL_PATH} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER})

if(EXTERNALPROJECT_C_FLAGS)
    set(COMMON_ARGS ${COMMON_ARGS} -DCMAKE_C_FLAGS=${EXTERNALPROJECT_C_FLAGS})
endif()

if(EXTERNALPROJECT_CXX_FLAGS)
    set(COMMON_ARGS ${COMMON_ARGS} -DCMAKE_CXX_FLAGS=${EXTERNALPROJECT_CXX_FLAGS})
endif()

# Create a Cmake library
#
function(set_library_path LIBRARY_NAME LIBRARY_PATH_VAR LIBRARY_FILE_PATH)
    if ("${LIBRARY_FILE_PATH}" STREQUAL "")
        set(LIBRARY_FILE_NAME ${CMAKE_STATIC_LIBRARY_PREFIX}${LIBRARY_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX})
    else()
        set(LIBRARY_FILE_NAME ${LIBRARY_FILE_PATH})
    endif()
    set(${LIBRARY_PATH_VAR} "${EXTERNAL_LIBRARIES_INSTALL_PATH}/lib/${LIBRARY_FILE_NAME}" PARENT_SCOPE)
endfunction()

# Setup imported libraries with the transitive cmake properties, so that you
# can simply target_link_libraries(mylib curl) and have it automatically
# receive the compile definitions, include paths, and all link dependencies.

macro(expose_external_library)
    add_library(${LIBRARY_NAME} STATIC IMPORTED GLOBAL)

    if (EXTERNAL_DEPENDENCY)
        add_dependencies(${LIBRARY_NAME} ${EXTERNAL_DEPENDENCY})
    endif(EXTERNAL_DEPENDENCY)

    if (LIBRARY_FILE_PATH)
        set_library_path(${LIBRARY_NAME} LIBRARY_PATH "${LIBRARY_FILE_PATH}")
    endif (LIBRARY_FILE_PATH)

    if (LIBRARY_PATH)
        set_property(TARGET ${LIBRARY_NAME} PROPERTY IMPORTED_LOCATION ${LIBRARY_PATH})
    endif(LIBRARY_PATH)

    if (LIBRARY_INCLUDE)
        set_property(TARGET ${LIBRARY_NAME} APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${LIBRARY_INCLUDE})
    endif (LIBRARY_INCLUDE)

    if(NOT "${LINK_DEPENDENCY}" STREQUAL "")
        set_property(TARGET ${LIBRARY_NAME} APPEND PROPERTY INTERFACE_LINK_LIBRARIES ${LINK_DEPENDENCY})
    endif(NOT "${LINK_DEPENDENCY}" STREQUAL "")

    if (SHOW_3RDPARTY_LIBRARIES_INFO)
        message("================ ${LIBRARY_NAME} ================")
        message("LIBRARY_NAME           = ${LIBRARY_NAME}")
        message("EXTERNAL_DEPENDENCY    = ${EXTERNAL_DEPENDENCY}")
        message("LIBRARY_FILE_PATH      = ${LIBRARY_FILE_PATH}")
        message("LIBRARY_PATH           = ${LIBRARY_PATH}")
        message("LIBRARY_INCLUDE        = ${LIBRARY_INCLUDE}")
        message("LINK_DEPENDENCY        = ${LINK_DEPENDENCY}")
    endif(SHOW_3RDPARTY_LIBRARIES_INFO)
endmacro()

function(expose_additional_external_library LIBRARY_NAME)
    if (ARGV1)
        set(LINK_DEPENDENCY ${ARGV1})
    endif(ARGV1)
    set_external_library(${LIBRARY_NAME} "${LINK_DEPENDENCY}" "${EXTERNAL_DEPENDENCY}")
    expose_external_library()
endfunction()

macro(set_external_library NAME)
    set(LIBRARY_NAME ${NAME})
    if (MSVC)
        set(LIBRARY_PATH ${EXTERNAL_LIBRARIES_INSTALL_PATH}/lib/${LIBRARY_NAME}.lib)
    else(MSVC)
        set(LIBRARY_PATH ${EXTERNAL_LIBRARIES_INSTALL_PATH}/lib/lib${LIBRARY_NAME}.a)
    endif(MSVC)
    set(LIBRARY_INCLUDE ${EXTERNAL_LIBRARIES_INSTALL_PATH}/include)

    set(LINK_DEPENDENCY ${ARGV1})
    set(EXTERNAL_DEPENDENCY ${ARGV2})
endmacro()