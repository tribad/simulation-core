include_directories(".")
include_directories("base")
include_directories("simifc")
include_directories("httpifc")
include_directories("components")
include_directories("components/messaging")
include_directories("components/threads")
include_directories("components/logging")
include_directories("components/cache")
include_directories("communication")

if (${ID} STREQUAL "opensuse-leap")
    include_directories("/usr/include/pgsql")
else()
    include_directories("/usr/include/postgresql")
endif()

add_subdirectory(base)
add_subdirectory(simifc)
add_subdirectory(httpifc)
add_subdirectory(components)
add_subdirectory(communication)
# add_subdirectory(htmlbase)
add_subdirectory(client)

add_library(simcore
    sim.cpp simworker.cpp timer.cpp commthread.cpp

    db.cpp

    mainctrl.cpp mainloadsimulation.cpp mainrunning.cpp mainstartsimdb.cpp
    mainstartstoragedb.cpp main.cpp
    )
