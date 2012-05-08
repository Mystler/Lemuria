find_path(CEGUI_INCLUDE_DIR CEGUI.h
    PATHS "/usr/local/include/CEGUI/"
    "/usr/include/CEGUI/"
)

find_library(CEGUI_LIBRARY
    NAMES CEGUIBase libCEGUIBase
    PATHS "/usr/local/lib/"
    "/usr/lib/"
)   
find_library(CEGUI_OGRERENDERER_LIBRARY
    NAMES CEGUIOgreRenderer libCEGUIOgreRenderer
    PATHS "/usr/local/lib/"
    "/usr/lib/"
)

set(CEGUI_LIBRARIES
    ${CEGUI_LIBRARY}
    ${CEGUI_OGRERENDERER_LIBRARY}
)

mark_as_advanced(
    CEGUI_LIBRARY
    CEGUI_OGRERENDERER_LIBRARY
)

if(CEGUI_INCLUDE_DIR AND CEGUI_LIBRARY AND CEGUI_OGRERENDERER_LIBRARY)
    set(CEGUI_FOUND TRUE)
endif()

if (CEGUI_FOUND)
    message(STATUS "Found CEGUI SDK")
endif()
