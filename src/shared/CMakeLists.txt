set(SHARED_HEADERS
    ntManager.h
    ntMessage.h
    ntPlayer.h
)

set(SHARED_SOURCES
    ntManager.cpp
    ntMessage.cpp
    ntPlayer.cpp
)

add_library(shared STATIC ${SHARED_HEADERS} ${SHARED_SOURCES})

source_group("Header Files" FILES ${SHARED_HEADERS})
source_group("Source Files" FILES ${SHARED_SOURCES})
