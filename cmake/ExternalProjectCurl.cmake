#============================================
# libcurl
#============================================

# External curl settings
set(curl_git_repo https://github.com/curl/curl.git)
set(curl_git_tag curl-7_50_3)
set(curl_external_project_name curl-build)

#-----------------------
# Import curl
#-----------------------
# After importing either through build or using an installed version the following vars are set:
# CURL_LIB_DIR, CURL_BIN_DIR and CURL_INC_DIR
if (BUILD_CURL)
    #-----------------------
    # Build curl from git
    #-----------------------
    # When building curl the CURL_INSTALL_DIR var is set to local build dir
    set(CURL_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/curl/install)
    ExternalProject_Add(${curl_external_project_name}
            PREFIX curl
            GIT_REPOSITORY ${curl_git_repo}
            GIT_TAG ${curl_git_tag}
            UPDATE_COMMAND ""
            CMAKE_ARGS -DBUILD_CURL_EXE=OFF -DBUILD_TESTING=OFF -DCMAKE_INSTALL_PREFIX:PATH=${CURL_INSTALL_DIR}
            )
    message("Building curl from github: ${curl_git_repo} ${curl_git_tag}")
    set(CURL_LIB_DIR ${CURL_INSTALL_DIR}/lib)

    if(WIN32)
        set(CURL_BIN_DIR ${CURL_INSTALL_DIR}/bin)
    endif()
    set(CURL_INC_DIR ${CURL_INSTALL_DIR}/include)

else()
    #-----------------------
    # Import curl from existing install
    #-----------------------
    # Two options here:
    # (1) Provide the following vars: CURL_LIB_DIR, CURL_BIN_DIR and CURL_INC_DIR
    # (2) Provide CURL_INSTALL_DIR and we expect the following structure
    #   CURL_INSTALL_DIR
    #      /lib               --> contains the library
    #      /bin               --> contains the dll (windows)
    #      /include           --> contains the header files
    #
    if ((NOT WIN32 AND DEFINED CURL_LIB_DIR AND DEFINED CURL_LIB_DIR) OR
    (WIN32     AND DEFINED CURL_LIB_DIR AND DEFINED CURL_LIB_DIR AND DEFINED CURL_BIN_DIR))
        message("Using CURL_LIB_DIR: ${CURL_LIB_DIR} and ${CURL_LIB_DIR} for curl" )
        if(NOT EXISTS ${CURL_LIB_DIR})
            message(FATAL_ERROR "CURL_LIB_DIR variable is defined but corresponds to non-existing directory")
        endif()
        if(NOT EXISTS ${CURL_INC_DIR})
            message(FATAL_ERROR "CURL_INC_DIR variable is defined but corresponds to non-existing directory")
        endif()

        if(WIN32)
            if(NOT EXISTS ${CURL_BIN_DIR})
                message(FATAL_ERROR "CURL_BIN_DIR variable is defined but corresponds to non-existing directory")
            endif()
        endif()
    elseif(DEFINED CURL_INSTALL_DIR)
        message("Using install dir: ${CURL_INSTALL_DIR} for curl" )
        if(NOT EXISTS ${CURL_INSTALL_DIR})
            message(FATAL_ERROR "CURL_INSTALL_DIR variable is defined but corresponds to non-existing directory")
        endif()
        set(CURL_LIB_DIR ${CURL_INSTALL_DIR}/lib)

        if(WIN32)
            set(CURL_BIN_DIR ${CURL_INSTALL_DIR}/bin)
        endif()
        set(CURL_INC_DIR ${CURL_INSTALL_DIR}/include)
    else()
        if(WIN32)
            message(FATAL_ERROR "CURL_INSTALL_DIR or (CURL_LIB_DIR and CURL_INC_DIR and CURL_BIN_DIR) not set")
        else()
            message(FATAL_ERROR "CURL_INSTALL_DIR or (CURL_LIB_DIR and CURL_INC_DIR) not set")
        endif()
    endif()
endif ()


#------------------------------
# curl IMPORTED library project
#------------------------------
include_directories(${CURL_INC_DIR})
add_library(curl SHARED IMPORTED GLOBAL)

#depending on platform we import the shared library and copy to the output
if(APPLE)
    set_property(TARGET curl PROPERTY IMPORTED_LOCATION ${CURL_LIB_DIR}/libcurl.dylib)
    if (BUILD_CURL)
        copy_to_binary_dir(${curl_external_project_name} ${CURL_LIB_DIR}/libcurl.dylib)
    endif()
elseif(UNIX)
    set_property(TARGET curl PROPERTY IMPORTED_LOCATION ${CURL_LIB_DIR}/libcurl.so)
    if (BUILD_CURL)
        copy_to_binary_dir(${curl_external_project_name} ${CURL_LIB_DIR}/libcurl.so)
    endif()
elseif(WIN32)
    set_property(TARGET curl PROPERTY IMPORTED_IMPLIB ${CURL_LIB_DIR}/libcurl_imp.lib)
    set_property(TARGET curl PROPERTY IMPORTED_LOCATION ${CURL_BIN_DIR}/libcurl.dll)
    if (BUILD_CURL)
        copy_to_binary_dir(${curl_external_project_name} ${CURL_BIN_DIR}/libcurl.dll)
    endif()
endif()

if (BUILD_CURL)
    #Needs to be set to force building of external project
    add_dependencies(curl ${curl_external_project_name})
endif()