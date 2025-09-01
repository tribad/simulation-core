include_directories("/usr/include/libxml2")
include_directories("../base")
include_directories("../components")
include_directories("../components/messaging")
include_directories("../components/threads")
include_directories(".")

add_compile_options (-fPIC)

add_library(simifc
    coreifc.cpp
    simifc.cpp simobjfactory.cpp
    simapi.cpp
    pointerindex.cpp
    ptrreleasethread.cpp
    memberbase.cpp membervalue.cpp membervaluearray.cpp
                   memberref.cpp   memberrefarray.cpp
    )
  
set_target_properties(simifc PROPERTIES OUTPUT_NAME simifc-${${UPPER_PROJECT_NAME}_VERSION_MAJOR}.${${UPPER_PROJECT_NAME}_VERSION_MINOR}.${${UPPER_PROJECT_NAME}_VERSION_PATCH})

install (TARGETS simifc DESTINATION lib64)
