project( oscada )


find_package( Threads REQUIRED)
#include_directories(${Threads_INCLUDE_DIRS})
set(LIBS ${LIBS} ${CMAKE_THREAD_LIBS_INIT})

#zlib
#symbols:   deflate
find_package( ZLIB REQUIRED)    
set(LIBS ${LIBS} ${ZLIB_LIBRARIES})

#pcre
#symbols:   pcre_free, pcre_exec, pcre_compile
find_package( PCRE REQUIRED)
#include_directories(${pcre_INCLUDE_DIRS})
set(LIBS ${LIBS} ${PCRE_LIBRARIES})


include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  #${TDE_INCLUDE_DIR}
  #${TQT_INCLUDE_DIRS}
)

link_directories(
  #${TQT_LIBRARY_DIRS}
)




set( SOURCES
    tsys.cpp resalloc.cpp tarchives.cpp tarchval.cpp tbds.cpp
    tvariant.cpp tconfig.cpp tcontroller.cpp tsecurity.cpp tmess.cpp
    tsubsys.cpp tprotocols.cpp ttransports.cpp tspecials.cpp ttypedaq.cpp
    tprmtmpl.cpp tvalue.cpp terror.cpp tuis.cpp tmodschedul.cpp telem.cpp
    tparamcontr.cpp tmodule.cpp xml.cpp tcntrnode.cpp tdaqs.cpp tfunction.cpp
)

##### oscada (shared) ###########################

add_library( ${PROJECT_NAME} SHARED
    ${SOURCES}
)

target_link_libraries (${PROJECT_NAME}
    ${CMAKE_DL_LIBS}                    ##dlopen/dlclose
    ${LIBS}
)
