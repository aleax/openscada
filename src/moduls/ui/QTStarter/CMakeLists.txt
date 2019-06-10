project(ui_QTStarter)

###http://doc.qt.io/qt-5/cmake-manual.html
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)
set(LIBS ${LIBS} Qt5::Widgets)

set(LIBS ${LIBS} oscada)

set( SOURCES
    tuimod.cpp
)

set( HEADERS
    tuimod.h
)

set( QRCs
    qtstarter.qrc
)

add_library( ${PROJECT_NAME} SHARED
    ${SOURCES} ${HEADERS}
)

target_link_libraries (${PROJECT_NAME}
    ${LIBS}
)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")    #no lib prefix: libui_QTStarter.so
