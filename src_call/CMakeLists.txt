project (openscada)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  #${TDE_INCLUDE_DIR}
  #${TQT_INCLUDE_DIRS}
  ../src
)

link_directories(
  #${TQT_LIBRARY_DIRS}
)

set( SOURCES
    main.cpp
)

##### openscada (executable) ####################

add_executable( openscada #AUTOMOC
  ${SOURCES}
  #DESTINATION ${BIN_INSTALL_DIR}
)

target_link_libraries(${PROJECT_NAME}
    oscada
)
