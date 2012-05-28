include_directories("..")

set(PLAYER_HEADERS
    crPlayer.h
    phAvatarController.h
)

set(PLAYER_SOURCES
    crPlayer.cpp
    phAvatarController.cpp
)

add_library(crPlayer STATIC ${PLAYER_SOURCES} ${PLAYER_HEADERS})

source_group("Source Files" FILES ${PLAYER_SOURCES})
source_group("Header Files" FILES ${PLAYER_HEADERS})
