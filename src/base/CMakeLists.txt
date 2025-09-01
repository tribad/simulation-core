include_directories("/usr/include/libxml2")
include_directories(".")

add_compile_options (-fPIC)

add_library(simbase
    simobj.cpp
    helper.cpp variant.cpp
    ptrnode.cpp ptrtreenode.cpp ptrleafnode.cpp ptrroot.cpp
    commtarget.cpp objectref.cpp
    CSystemTime.cpp
    CIdToObject.cpp CStringSet.cpp CStringToAny.cpp CStringToObject.cpp
    CDirectory.cpp CFile.cpp CFileInfo.cpp CFileName.cpp CSharedLibrary.cpp sigmanager.cpp
    Pattern.cpp
    netpack.cpp msg.cpp
    json.cpp sqlmap.cpp
    )
  
set_target_properties(simbase PROPERTIES OUTPUT_NAME simbase-${${UPPER_PROJECT_NAME}_VERSION_MAJOR}.${${UPPER_PROJECT_NAME}_VERSION_MINOR}.${${UPPER_PROJECT_NAME}_VERSION_PATCH})

install (TARGETS simbase DESTINATION lib64)
