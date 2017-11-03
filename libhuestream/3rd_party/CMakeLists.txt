cmake_minimum_required(VERSION 3.6)

include(LibraryRepo)

add_subdirectory(libjson)

# set external project paths
set(EXTERNAL_LIBRARIES_BUILD_PATH ${PROJECT_BINARY_DIR}/external_build)
set(EXTERNAL_LIBRARIES_INSTALL_PATH ${PROJECT_BINARY_DIR}/external_install)
set(EXTERNAL_LIBRARIES_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set_library_repo("${3RDPARTY_LIBRARIES_REPO_URL}" "${3RDPARTY_LIBRARIES_ARCHIVE_FILEPATH}" "3rdParty" "1.0" ${EXTERNAL_LIBRARIES_INSTALL_PATH})

# create empty instalation folder in order to avoid cmake's includes non-existen path warnings
file(MAKE_DIRECTORY ${EXTERNAL_LIBRARIES_INSTALL_PATH}/include)

# download and compile external projects
include(ExternalGoogleTest)
include(ExternalMbedtls)
include(ExternalEdtls)
include(ExternalCurl)