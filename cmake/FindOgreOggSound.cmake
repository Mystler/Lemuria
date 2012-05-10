find_path(OGREOGGSOUND_INCLUDE_DIR OgreOggSound.h
    PATHS "/usr/local/include/OgreOggSound/"
    "/usr/include/OgreOggSound/"
)

find_library(OGREOGGSOUND_LIBRARY
    NAMES OgreOggSound libOgreOggSound
    PATHS "/usr/local/lib/"
    "/usr/lib/"
)

if(OGREOGGSOUND_INCLUDE_DIR AND OGREOGGSOUND_LIBRARY)
    set(OGREOGGSOUND_FOUND TRUE)
endif()

if (OGREOGGSOUND_FOUND)
    message(STATUS "Found OgreOggSound")
endif()
