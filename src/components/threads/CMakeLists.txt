include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_compile_options (-fPIC)

add_library(threads
            CEvent.cpp
            CMutex.cpp
            CThread.cpp
            CMicroThread.cpp
            CMicroManagerThread.cpp
)

set_target_properties(threads PROPERTIES OUTPUT_NAME simthreads-${${UPPER_PROJECT_NAME}_VERSION_MAJOR}.${${UPPER_PROJECT_NAME}_VERSION_MINOR}.${${UPPER_PROJECT_NAME}_VERSION_PATCH})

install (TARGETS threads DESTINATION lib64)
