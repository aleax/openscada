project (openscada)

set(LIBS ${LIBS} oscada)


include_directories(
#  ${CMAKE_BINARY_DIR}
#  ${CMAKE_CURRENT_BINARY_DIR}
#  ${CMAKE_CURRENT_SOURCE_DIR}
#  #${TDE_INCLUDE_DIR}
#  #${TQT_INCLUDE_DIRS}
#  ../src
)

#link_directories(
  #${TQT_LIBRARY_DIRS}
#)

set( SOURCES
    main.cpp
)

add_executable( ${PROJECT_NAME}
  ${SOURCES}
)

target_link_libraries(${PROJECT_NAME}
    ${LIBS}
)

install (TARGETS ${PROJECT_NAME} DESTINATION bin)
