include_directories("..")

set(MESSAGE_HEADERS
    ntNetMgr.h
    ntPlayer.h
)

set(MESSAGE_SOURCES
    ntNetMgr.cpp
    ntPlayer.cpp
)

add_library(ntNetwork STATIC ${MESSAGE_SOURCES} ${MESSAGE_HEADERS})

source_group("Source Files" FILES ${MESSAGE_SOURCES})
source_group("Header Files" FILES ${MESSAGE_HEADERS})
