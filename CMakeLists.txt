cmake_minimum_required(VERSION 2.8)

project(Lemuria)

if(WIN32)
    set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;$ENV{OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
endif()

if(UNIX)
    if(EXISTS "/usr/local/lib/OGRE/cmake")
        set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;/usr/local/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
    elseif(EXISTS "/usr/lib/OGRE/cmake")
        set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;/usr/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
    else()
        message(SEND_ERROR "Failed to find module path.")
    endif()
endif()

set(CMAKE_DEBUG_POSTFIX "_d")
set(Boost_USE_STATIC_LIBS TRUE)

find_package(OGRE REQUIRED)
find_package(OIS REQUIRED)
find_package(Boost COMPONENTS thread date_time REQUIRED)
find_package(CEGUI REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OgreOggSound REQUIRED)
find_package(Bullet REQUIRED)
find_package(RakNet REQUIRED)

if(UNIX)
    find_package(Threads)
endif()

include_directories(${OIS_INCLUDE_DIRS}
    ${OGRE_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIR}
    ${CEGUI_INCLUDE_DIR}
    ${OPENAL_INCLUDE_DIR}
    ${OGREOGGSOUND_INCLUDE_DIR}
    ${BULLET_INCLUDE_DIRS}
    ${RAKNET_INCLUDE_DIR}
)

add_subdirectory(src)
