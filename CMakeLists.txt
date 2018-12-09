cmake_minimum_required(VERSION 3.10)

##
##AC_INIT(OpenSCADA, [0.9.0], rom_as@oscada.org)
##produces defenitions:
#| #define PACKAGE_NAME "OpenSCADA"
#| #define PACKAGE_TARNAME "openscada"
#| #define PACKAGE_VERSION "0.9.0"
#| #define PACKAGE_STRING "OpenSCADA 0.9.0"
#| #define PACKAGE_BUGREPORT "rom_as@oscada.org"
#| #define PACKAGE_URL ""
#| #define PACKAGE "openscada"
#| #define VERSION "0.9.0"
project(openscada)

set( PACKAGE_NAME "OpenSCADA" )
set( PACKAGE_TARNAME ${CMAKE_PROJECT_NAME} ) 
set( PACKAGE_VERSION "0.9.0"  )
set( PACKAGE_STRING "OpenSCADA 0.9.0" )
set( PACKAGE_BUGREPORT "rom_as@oscada.org" )
set( PACKAGE_URL "" )
set( PACKAGE ${CMAKE_PROJECT_NAME} )
set( VERSION "0.9.0" )

#own cmake modules
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")


#configuration

#variables from configure.ac
#oscd_moduldir=\${libdir}/${PACKAGE}
#oscd_datadir=\${datadir}/${PACKAGE}
#oscd_moddir_full=${libdir}/${PACKAGE}
#oscd_datadir_full=${datadir}/${PACKAGE}
#AC_SUBST(oscd_LIBREVISION)
#AC_SUBST(oscd_moduldir)
#AC_SUBST(oscd_modulpref)
#AC_SUBST(oscd_datadir)
#AX_DEFINE_DIR(bindir_full, bindir, [Binaries location for configurations])
#AX_DEFINE_DIR(sysconfdir_full, sysconfdir, [Configure location for configurations])
#AX_DEFINE_DIR(libdir_full, libdir, [Libraries location for configurations])
#AX_DEFINE_DIR(datadir_full, datadir, [Data location for configurations])
#AX_DEFINE_DIR(localedir_full, localedir, [I18N locales location for configurations])
#AX_DEFINE_DIR(oscd_datadir_full, oscd_datadir_full, [OpenSCADA data location for configurations])
#AX_DEFINE_DIR(oscd_moddir_full, oscd_moddir_full, [OpenSCADA modules location for configurations])

include( GNUInstallDirs )

set( prefix ${CMAKE_INSTALL_PREFIX} )

add_definitions(-Dsysconfdir_full="${CMAKE_INSTALL_FULL_SYSCONFDIR}")
set( libdir_full ${CMAKE_INSTALL_FULL_LIBDIR} )
add_definitions(-Dlibdir_full="${libdir_full}")
add_definitions(-Doscd_moddir_full="${CMAKE_INSTALL_FULL_LIBDIR}/${CMAKE_PROJECT_NAME}")
add_definitions(-Doscd_datadir_full="${CMAKE_INSTALL_FULL_DATADIR}/${CMAKE_PROJECT_NAME}")
add_definitions(-Ddatadir_full="${CMAKE_INSTALL_FULL_DATADIR}")
add_definitions(-Dbindir_full="${CMAKE_INSTALL_FULL_BINDIR}")
add_definitions(-DPACKAGE_NAME="${PACKAGE_NAME}")
add_definitions(-DPACKAGE="${PACKAGE}")
add_definitions(-DVERSION="${VERSION}")
add_definitions(-DMODS_INCL="${ModsIncl}")  #TODO

### https://gitlab.kitware.com/cmake/community/wikis/doc/cmake/RPATH-handling
# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")



add_subdirectory( src )
add_subdirectory( src_call )
add_subdirectory( src/moduls )

###https://stackoverflow.com/questions/17511496/how-to-create-a-shared-library-with-cmake
configure_file(openscada.pc.in openscada.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/openscada.pc
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig)
