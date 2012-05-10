set(SERVER
    svMain.cpp
)

add_executable(AkshahNet ${SERVER})

target_link_libraries(AkshahNet ${RAKNET_LIBRARY})

if(UNIX)
    target_link_libraries(AkshahNet ${CMAKE_THREAD_LIBS_INIT})
endif()

if(LEMURIA_INSTALL_EXE)
    install(TARGETS AkshahNet DESTINATION bin)
endif()
