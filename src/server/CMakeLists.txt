include_directories("../crPlayer")
include_directories("../ntMessage")
include_directories("..")

set(SERVER_HEADERS
    svServer.h
)

set(SERVER_SOURCES
    svServer.cpp
)

add_executable(AkshahNet
    ${SERVER_HEADERS}
    ${SERVER_SOURCES}
)

target_link_libraries(AkshahNet
    ${RAKNET_LIBRARY}
    ${Boost_LIBRARIES}
    crPlayer
    ntMessage
)

if(UNIX)
    target_link_libraries(AkshahNet ${CMAKE_THREAD_LIBS_INIT})
endif()

if(LEMURIA_INSTALL_EXE)
    install(TARGETS AkshahNet DESTINATION bin)
endif()

source_group("Header Files" FILES ${SERVER_HEADERS})
source_group("Source Files" FILES ${SERVER_SOURCES})
