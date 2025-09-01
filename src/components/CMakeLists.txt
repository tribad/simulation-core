include_directories("/usr/include/libxml2")
include_directories(".")

add_subdirectory(messaging)
add_subdirectory(threads)
add_subdirectory(logging)
add_subdirectory(pgsql)
add_subdirectory(cache)
