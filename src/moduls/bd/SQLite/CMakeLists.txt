project(bd_SQLite)



#include_directories(
#  ${CMAKE_BINARY_DIR}
#  ${CMAKE_CURRENT_BINARY_DIR}
#  ${CMAKE_CURRENT_SOURCE_DIR}
#  #${TDE_INCLUDE_DIR}
#  #${TQT_INCLUDE_DIRS}
#)

#link_directories(
#  #${TQT_LIBRARY_DIRS}
#)

set(LIBS ${LIBS} oscada)



set( SOURCES
    bd_sqlite.cpp
)

set( HEADERS
    bd_sqlite.h
)

##### oscada (shared) ###########################

add_library( ${PROJECT_NAME} SHARED
    ${SOURCES} ${HEADERS}
)

target_link_libraries (${PROJECT_NAME}
    ${LIBS}
)


set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")    #no lib prefix: libbd_SQLite.so
