find_path(RAKNET_INCLUDE_DIR RakPeerInterface.h
    PATHS "/usr/local/include/raknet/"
    "/usr/include/raknet/"
)

find_library(RAKNET_LIBRARY
    NAMES RakNetStatic libRakNetStatic
    PATHS "/usr/local/lib/"
    "/usr/lib/"
)

if(RAKNET_INCLUDE_DIR AND RAKNET_LIBRARY)
    set(RAKNET_FOUND TRUE)
endif()

if (RAKNET_FOUND)
    message(STATUS "RakNet found!")
endif()
