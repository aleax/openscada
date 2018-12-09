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

#symbols: dlopen, dlclose
set(LIBS ${LIBS} ${CMAKE_DL_LIBS} )

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
)




set( SOURCES
    tsys.cpp resalloc.cpp tarchives.cpp tarchval.cpp tbds.cpp
    tvariant.cpp tconfig.cpp tcontroller.cpp tsecurity.cpp tmess.cpp
    tsubsys.cpp tprotocols.cpp ttransports.cpp tspecials.cpp ttypedaq.cpp
    tprmtmpl.cpp tvalue.cpp terror.cpp tuis.cpp tmodschedul.cpp telem.cpp
    tparamcontr.cpp tmodule.cpp xml.cpp tcntrnode.cpp tdaqs.cpp tfunction.cpp
)

set ( HEADERS
    autohd.h ieee754.h resalloc.h tarchives.h tarchval.h tbds.h tcntrnode.h
    tconfig.h tcontroller.h tdaqs.h telem.h terror.h tfunction.h tmess.h 
    tmodschedul.h tmodule.h tparamcontr.h tprmtmpl.h tprotocols.h tsecurity.h
    tspecials.h tsubsys.h tsys.h ttransports.h ttypedaq.h ttypeparam.h tuis.h
    tvalue.h tvariant.h xml.h
)

##### oscada (shared) ###########################

add_library( ${PROJECT_NAME} SHARED
    ${SOURCES} ${HEADERS}
)

target_link_libraries (${PROJECT_NAME}
    ${LIBS}
)

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}
)
