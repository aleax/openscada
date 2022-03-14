# - Try to find libopenscada
# Once done this will define
#  libopenscada_FOUND - System has libopenscada
#  libopenscada_INCLUDE_DIRS - The libopenscada include directories
#  libopenscada_LIBRARIES - The libraries needed to use libopenscada
#  libopenscada_DEFINITIONS - Compiler switches required for using libopenscada

# If libopenscada_ROOT_DIR was defined in the environment, use it.
IF(NOT libopenscada_ROOT_DIR AND NOT $ENV{libopenscada_ROOT_DIR} STREQUAL "")
    SET(libopenscada_ROOT_DIR $ENV{libopenscada_ROOT_DIR})
ENDIF()

SET(_libopenscada_SEARCH_DIRS
    ${libopenscada_ROOT_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
)

message("_libopenscada_SEARCH_DIRS: ${_libopenscada_SEARCH_DIRS}")

FIND_PATH(libopenscada_INCLUDE_DIR tsys.h
    HINTS
        ${_libopenscada_SEARCH_DIRS}
)

FIND_LIBRARY(libopenscada_LIBRARY
    NAMES
        openscada
    HINTS
        ${_libopenscada_SEARCH_DIRS}
    PATH_SUFFIXES
        lib64 lib
)

# handle the QUIETLY and REQUIRED arguments and set PCRE_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(libopenscada DEFAULT_MSG
    libopenscada_LIBRARY libopenscada_INCLUDE_DIR)

IF(libopenscada_FOUND)
    SET(libopenscada_LIBRARIES ${libopenscada_LIBRARY})
    SET(libopenscada_INCLUDE_DIRS ${libopenscada_INCLUDE_DIR})
ENDIF(libopenscada_FOUND)

MARK_AS_ADVANCED(
    libopenscada_INCLUDE_DIR
    libopenscada_LIBRARY
)
