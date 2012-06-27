add_subdirectory(server)
add_subdirectory(shared)

set(CLIENT_HEADERS
    crClient.h
    crGUIManager.h
    phAvatarController.h
    phBullet.h
    scObjectMgr.h
)

set(CLIENT_SOURCES
    crClient.cpp
    crKeyMgr.cpp
    crMouseMgr.cpp
    crGUIManager.cpp
    phAvatarController.cpp
    phBullet.cpp
    scObjectMgr.cpp
)

set(3rdparty
    3rdparty/tinyxml/tinyxml.h
    3rdparty/tinyxml/tinyxml.cpp
    3rdparty/tinyxml/tinyxmlerror.cpp
    3rdparty/tinyxml/tinyxmlparser.cpp
    3rdparty/tinyxml/tinystr.h
    3rdparty/tinyxml/tinystr.cpp

    3rdparty/ogremax/OgreMaxModel.hpp
    3rdparty/ogremax/OgreMaxModel.cpp
    3rdparty/ogremax/OgreMaxPlatform.hpp
    3rdparty/ogremax/OgreMaxRenderWindowIterator.hpp
    3rdparty/ogremax/OgreMaxScene.hpp
    3rdparty/ogremax/OgreMaxScene.cpp
    3rdparty/ogremax/OgreMaxTypes.hpp
    3rdparty/ogremax/OgreMaxUtilities.hpp
    3rdparty/ogremax/OgreMaxUtilities.cpp
    3rdparty/ogremax/ProgressCalculator.hpp
    3rdparty/ogremax/ProgressCalculator.cpp
    3rdparty/ogremax/Version.hpp
    3rdparty/ogremax/Version.cpp

    3rdparty/btogre/BtOgre.cpp
    3rdparty/btogre/BtOgreExtras.h
    3rdparty/btogre/BtOgreGP.h
    3rdparty/btogre/BtOgrePG.h
)

add_definitions(-DTIXML_USE_STL)

add_executable(Lemuria WIN32
    ${CLIENT_HEADERS}
    ${CLIENT_SOURCES}
    ${3rdparty}
)

target_link_libraries(Lemuria
    ${OGRE_LIBRARIES}
    ${OIS_LIBRARIES}
    ${Boost_LIBRARIES}
    ${CEGUI_LIBRARIES}
    ${OPENAL_LIBRARY}
    ${OGREOGGSOUND_LIBRARY}
    ${BULLET_LIBRARIES}
    ${RAKNET_LIBRARY}
    shared
)

if(UNIX)
    target_link_libraries(Lemuria ${CMAKE_THREAD_LIBS_INIT})
endif()

source_group("Header Files" FILES ${CLIENT_HEADERS})
source_group("Source Files" FILES ${CLIENT_SOURCES})
source_group("3rdparty" FILES ${3rdparty})

option(LEMURIA_INSTALL_EXE "Install executables" TRUE)
option(LEMURIA_INSTALL_DATA "Install data files" TRUE)
option(LEMURIA_INSTALL_CONFIG "Install config files" FALSE)

if(LEMURIA_INSTALL_EXE)
    install(TARGETS Lemuria DESTINATION bin)
endif()

if(LEMURIA_INSTALL_DATA)
    install(DIRECTORY
        ../data/dat
        DESTINATION .
    )
endif()

if(LEMURIA_INSTALL_CONFIG)
    install(DIRECTORY
        ../data/cfg
        DESTINATION .
    )
endif()
