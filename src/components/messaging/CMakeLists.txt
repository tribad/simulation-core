include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories("../threads")

add_library(msg 
            msgqueue.cpp
            sharedmsgqueue.cpp
            msgthread.cpp
            sharedmsgthread.cpp
)

set_target_properties(msg PROPERTIES OUTPUT_NAME msg-${${UPPER_PROJECT_NAME}_VERSION_MAJOR}.${${UPPER_PROJECT_NAME}_VERSION_MINOR}.${${UPPER_PROJECT_NAME}_VERSION_PATCH})

install (TARGETS msg DESTINATION lib64)
