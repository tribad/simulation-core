##
##  This is the cmake intro
cmake_minimum_required(VERSION 3.10)
##
##  Abort if in source build
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message( FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt." )
endif()
##
##  Setting the project name
project(appcore)
string(TOUPPER ${PROJECT_NAME} UPPER_PROJECT_NAME)
if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()
##
##  Load values from the os-release information.
if (EXISTS "/etc/os-release")
    file(STRINGS "/etc/os-release" OSINFO)
    foreach(NameAndValue ${OSINFO})
        # Strip leading spaces
        string(REGEX REPLACE "^[ ]+" "" NameAndValue ${NameAndValue})
        # Find variable name
        string(REGEX MATCH "^[^=]+" Name ${NameAndValue})
        # Find the value
        string(REPLACE "${Name}=" "" Value ${NameAndValue})
        string(REPLACE "\"" "" Value ${Value})
        # Set the variable
        set(${Name} "${Value}")
    endforeach()
else()
    message(WARNING "os-release info not found")
endif()
##
##  Load povmodeler version number
if (EXISTS ${CMAKE_SOURCE_DIR}/VERSION)
    file(STRINGS ${CMAKE_SOURCE_DIR}/VERSION VINFO)
    foreach(NameAndValue ${VINFO})
        # Strip leading spaces
        string(REGEX REPLACE "^[ ]+" "" NameAndValue ${NameAndValue})
        # Find variable name
        string(REGEX MATCH "^[^=]+" Name ${NameAndValue})
        # Find the value
        string(REPLACE "${Name}=" "" Value ${NameAndValue})
        # Set the variable
        set(${Name} "${Value}")
    endforeach()
    set(${UPPER_PROJECT_NAME}_VERSION_MAJOR "${VERSION_MAJOR}")
    set(${UPPER_PROJECT_NAME}_VERSION_MINOR "${VERSION_MINOR}")
    set(${UPPER_PROJECT_NAME}_VERSION_PATCH "${VERSION_PATCH}")
else()
    message(WARNING "version info not found")
    set(${UPPER_PROJECT_NAME}_VERSION_MAJOR "0")
    set(${UPPER_PROJECT_NAME}_VERSION_MINOR "0")
    set(${UPPER_PROJECT_NAME}_VERSION_PATCH "0")
endif()
##
##  Add the postgresql dependency.
find_package(PostgreSQL REQUIRED)
##
##  Create the version header.
configure_file(version.h.in ${CMAKE_SOURCE_DIR}/src/version.h)
##
##  This is the application install dir. Set fixed for now. Should be changed.
set (XDG_APPS_INSTALL_DIR /usr/local/bin)
##
##  Setup the global compile options for now only debug is supported.
add_compile_options (-O0 -g -Wall -std=c++17 -Wextra)
## add_compile_options (-O0 -g -Wall -std=c++17 -Wextra -fsanitize=address -fno-omit-frame-pointer)
## add_compile_options (-O3 -Wall -std=c++17 -Wextra)
##
##  setting the base dir to be included in the include path list.
include_directories(${CMAKE_SOURCE_DIR} /usr/include/libxml2 ${CMAKE_SOURCE_DIR}/src/communication/pdu )
##
## set the executable name
set (PROJECT_EXEC_NAME ${PROJECT_NAME}-${${UPPER_PROJECT_NAME}_VERSION_MAJOR}.${${UPPER_PROJECT_NAME}_VERSION_MINOR}.${${UPPER_PROJECT_NAME}_VERSION_PATCH})
##
##  take the source
add_subdirectory(src)
##
##  Setup the executable name.
add_executable(${PROJECT_NAME} "/dev/null")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}-${${UPPER_PROJECT_NAME}_VERSION_MAJOR}.${${UPPER_PROJECT_NAME}_VERSION_MINOR}.${${UPPER_PROJECT_NAME}_VERSION_PATCH})
##
##  What to link to make this executable.
target_link_libraries(${PROJECT_NAME} -Wl,--start-group client simbase simifc simcore msg threads comm httpifc logger cache xml2 gnutls pq pthread dl rt -Wl,--end-group PostgreSQL::PostgreSQL)
##
##  Files to install
install ( FILES
         ${CMAKE_SOURCE_DIR}/src/base/CDirectory.h
         ${CMAKE_SOURCE_DIR}/src/base/CFile.h
         ${CMAKE_SOURCE_DIR}/src/base/CFileInfo.h
         ${CMAKE_SOURCE_DIR}/src/base/CFileName.h
         ${CMAKE_SOURCE_DIR}/src/base/CIdToObject.h
         ${CMAKE_SOURCE_DIR}/src/base/CSharedLibrary.h
         ${CMAKE_SOURCE_DIR}/src/base/CStringSet.h
         ${CMAKE_SOURCE_DIR}/src/base/CStringToAny.h
         ${CMAKE_SOURCE_DIR}/src/base/CStringToObject.h
         ${CMAKE_SOURCE_DIR}/src/base/CSystemTime.h
         ${CMAKE_SOURCE_DIR}/src/base/commtarget.h
         ${CMAKE_SOURCE_DIR}/src/base/LockedSet.h
         ${CMAKE_SOURCE_DIR}/src/base/LockedList.h
         ${CMAKE_SOURCE_DIR}/src/base/LockedMap.h
         ${CMAKE_SOURCE_DIR}/src/base/LockedVector.h
         ${CMAKE_SOURCE_DIR}/src/base/LockedType.h
         ${CMAKE_SOURCE_DIR}/src/simifc/coreifc.h
         ${CMAKE_SOURCE_DIR}/src/base/helper.h
         ${CMAKE_SOURCE_DIR}/src/httpifc/httpcontentifc.h
         ${CMAKE_SOURCE_DIR}/src/communication/pdu/httprequest.h
         ${CMAKE_SOURCE_DIR}/src/communication/pdu/httpresponse.h
         ${CMAKE_SOURCE_DIR}/src/httpifc/httpsimulationifc.h
         ${CMAKE_SOURCE_DIR}/src/base/json.h
         ${CMAKE_SOURCE_DIR}/src/base/msg.h
         ${CMAKE_SOURCE_DIR}/src/simifc/msgdb.h
         ${CMAKE_SOURCE_DIR}/src/base/netpack.h
         ${CMAKE_SOURCE_DIR}/src/simifc/pointerindex.h
         ${CMAKE_SOURCE_DIR}/src/base/ptrnode.h
         ${CMAKE_SOURCE_DIR}/src/simifc/simapi.h
         ${CMAKE_SOURCE_DIR}/src/simifc/membervaluearray.h
         ${CMAKE_SOURCE_DIR}/src/simifc/memberbase.h
         ${CMAKE_SOURCE_DIR}/src/simifc/membervalue.h
         ${CMAKE_SOURCE_DIR}/src/simifc/memberref.h
         ${CMAKE_SOURCE_DIR}/src/simifc/membertypes.h
         ${CMAKE_SOURCE_DIR}/src/simifc/memberrefarray.h
         ${CMAKE_SOURCE_DIR}/src/simifc/simifc.h
         ${CMAKE_SOURCE_DIR}/src/base/simobj.h
         ${CMAKE_SOURCE_DIR}/src/base/sqlmap.h
         ${CMAKE_SOURCE_DIR}/src/base/sqlrecordset.h
         ${CMAKE_SOURCE_DIR}/src/simifc/simobjfactory.h
         ${CMAKE_SOURCE_DIR}/src/base/variant.h
         ${CMAKE_SOURCE_DIR}/src/base/objectref.h
         ${CMAKE_SOURCE_DIR}/src/simifc/tMsgEndCycleReq.h
         ${CMAKE_SOURCE_DIR}/src/simifc/tSignalEndCycle.h
         ${CMAKE_SOURCE_DIR}/src/simifc/tMsgSQLExecStringReply.h
         ${CMAKE_SOURCE_DIR}/src/simifc/tMsgSQLExecStringReq.h
         ${CMAKE_SOURCE_DIR}/src/simifc/tMsgStartCycleReq.h
         ${CMAKE_SOURCE_DIR}/src/simifc/tSignalStartCycle.h
         ${CMAKE_SOURCE_DIR}/src/simifc/tMsgInvalidReply.h
         ${CMAKE_SOURCE_DIR}/src/simifc/tMsgWSConnectReply.h
         ${CMAKE_SOURCE_DIR}/src/simifc/tMsgWSConnectReq.h
         ${CMAKE_SOURCE_DIR}/src/simifc/tMsgWSDisconnectReply.h
         ${CMAKE_SOURCE_DIR}/src/simifc/tMsgWSDisconnectReq.h
         ${CMAKE_SOURCE_DIR}/src/simifc/tSigSQLConnectReply.h
         ${CMAKE_SOURCE_DIR}/src/simifc/tSigInvalidIndication.h
         ${CMAKE_SOURCE_DIR}/src/simifc/tSigSQLConnectReq.h
         ${CMAKE_SOURCE_DIR}/src/simifc/tSigSQLDisconnectReply.h
         ${CMAKE_SOURCE_DIR}/src/simifc/tSigSQLDisconnectReq.h
         ${CMAKE_SOURCE_DIR}/src/simifc/tSigSQLExecStringReply.h
         ${CMAKE_SOURCE_DIR}/src/simifc/tSigSQLExecStringReq.h
         ${CMAKE_SOURCE_DIR}/src/simifc/tSigSQLFetchReply.h
         ${CMAKE_SOURCE_DIR}/src/simifc/tSigSQLFetchReq.h
         ${CMAKE_SOURCE_DIR}/src/components/tSigPrepareStorageReply.h
         ${CMAKE_SOURCE_DIR}/src/components/tSigPrepareStorageReq.h
         ${CMAKE_SOURCE_DIR}/src/simifc/tSigWSDisconnectIndication.h
         ${CMAKE_SOURCE_DIR}/src/simifc/tSigRecvRawIndication.h
         ${CMAKE_SOURCE_DIR}/src/simifc/tSigSendRawIndication.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CArea.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CArticle.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CASide.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CAudio.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CBody.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CButton.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CCanvas.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CDataList.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CDiv.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CEmbed.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CEventHandler.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CFieldSet.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CFigure.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CFooter.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CForm.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CHeader.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CHead.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CHtml.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CHtmlObject.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CIFrame.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CImage.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CInput.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CLabel.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CLink.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CListEntry.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CList.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CMap.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CMeta.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CMeter.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CNav.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CObject.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/COptionGroup.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/COption.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/COutput.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CPlainText.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CProgress.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CSection.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CSelect.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CSource.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CSVG.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CTextArea.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CTrack.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CVideo.h
         ${CMAKE_SOURCE_DIR}/src/htmlbase/CHtmlPage.h
         ${CMAKE_SOURCE_DIR}/src/components/logging/logger.h
         ${CMAKE_SOURCE_DIR}/src/components/threads/CEvent.h
         ${CMAKE_SOURCE_DIR}/src/components/threads/CMutex.h
         ${CMAKE_SOURCE_DIR}/src/components/threads/CThread.h
         ${CMAKE_SOURCE_DIR}/src/components/cache/cachethread.h
         ${CMAKE_SOURCE_DIR}/src/components/cache/cacheentry.h
         ${CMAKE_SOURCE_DIR}/src/components/messaging/msgqueue.h
         ${CMAKE_SOURCE_DIR}/src/components/messaging/sharedmsgqueue.h
         ${CMAKE_SOURCE_DIR}/src/components/messaging/msgthread.h
         ${CMAKE_SOURCE_DIR}/src/components/messaging/sharedmsgthread.h
         ${CMAKE_SOURCE_DIR}/src/components/dbmsg.h

DESTINATION include/appcore/${${UPPER_PROJECT_NAME}_VERSION_MAJOR}.${${UPPER_PROJECT_NAME}_VERSION_MINOR}.${${UPPER_PROJECT_NAME}_VERSION_PATCH})

install ( FILES
        ${CMAKE_SOURCE_DIR}/src/client/socketthread.h
        ${CMAKE_SOURCE_DIR}/src/client/socket.h
        ${CMAKE_SOURCE_DIR}/src/client/tcpsocket.h
        ${CMAKE_SOURCE_DIR}/src/client/websocket.h
        ${CMAKE_SOURCE_DIR}/src/client/wsmessage.h
        ${CMAKE_SOURCE_DIR}/src/client/http-proto-response.h

        DESTINATION include/appcore/${${UPPER_PROJECT_NAME}_VERSION_MAJOR}.${${UPPER_PROJECT_NAME}_VERSION_MINOR}.${${UPPER_PROJECT_NAME}_VERSION_PATCH}/client)


install ( TARGETS  ${PROJECT_NAME} DESTINATION bin)

