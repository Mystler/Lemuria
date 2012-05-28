include_directories("..")

set(MESSAGE_HEADERS
    ntNetMgr.h
)

set(MESSAGE_SOURCES
    ntNetMgr.cpp
)

add_library(ntMessage STATIC ${MESSAGE_SOURCES} ${MESSAGE_HEADERS})

source_group("Source Files" FILES ${MESSAGE_SOURCES})
source_group("Header Files" FILES ${MESSAGE_HEADERS})
