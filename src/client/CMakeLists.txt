include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/comm)
include_directories("../components/threads")
include_directories("../components/logging")

add_compile_options (-fPIC)

add_library(client
    socket.cpp
    tcpsocket.cpp
    http-proto-response.cpp
    websocket.cpp
    wsmessage.cpp
    socketthread.cpp
)

set_target_properties(client PROPERTIES OUTPUT_NAME client-${${UPPER_PROJECT_NAME}_VERSION_MAJOR}.${${UPPER_PROJECT_NAME}_VERSION_MINOR}.${${UPPER_PROJECT_NAME}_VERSION_PATCH})

install (TARGETS client DESTINATION lib64)
