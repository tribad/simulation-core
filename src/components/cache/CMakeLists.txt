include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_compile_options (-fPIC)

add_library(cache
            cachethread.cpp
            cacheentry.cpp
)

set_target_properties(cache PROPERTIES OUTPUT_NAME simcache-${${UPPER_PROJECT_NAME}_VERSION_MAJOR}.${${UPPER_PROJECT_NAME}_VERSION_MINOR}.${${UPPER_PROJECT_NAME}_VERSION_PATCH})

install (TARGETS cache DESTINATION lib64)
