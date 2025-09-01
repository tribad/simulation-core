include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(".")

add_compile_options (-fPIC)

add_library(httpifc
            httpcontentifc.cpp httpsimulationifc.cpp httpcontentlib.cpp
)
set_target_properties(httpifc PROPERTIES OUTPUT_NAME httpifc-${${UPPER_PROJECT_NAME}_VERSION_MAJOR}.${${UPPER_PROJECT_NAME}_VERSION_MINOR}.${${UPPER_PROJECT_NAME}_VERSION_PATCH})

install ( TARGETS httpifc DESTINATION lib64)
