include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories("../threads")

add_compile_options (-fPIC)

add_library(logger
            logger.cpp
            logthread.cpp
)

set_target_properties(logger PROPERTIES OUTPUT_NAME logger-${${UPPER_PROJECT_NAME}_VERSION_MAJOR}.${${UPPER_PROJECT_NAME}_VERSION_MINOR}.${${UPPER_PROJECT_NAME}_VERSION_PATCH})

install (TARGETS logger DESTINATION lib64)
