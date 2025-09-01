include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories("../threads")

add_library(pgsqlstorage MODULE
            objdb.cpp storage.cpp
)

set_target_properties(pgsqlstorage PROPERTIES OUTPUT_NAME pgsqlstorage-${${UPPER_PROJECT_NAME}_VERSION_MAJOR}.${${UPPER_PROJECT_NAME}_VERSION_MINOR}.${${UPPER_PROJECT_NAME}_VERSION_PATCH})

target_link_libraries(pgsqlstorage PostgreSQL::PostgreSQL)

install (TARGETS pgsqlstorage DESTINATION lib64)
