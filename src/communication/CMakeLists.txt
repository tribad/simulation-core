include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/pdu)
include_directories("../components/threads")
include_directories("../components/logging")
include_directories("../httpifc" "pdu")

add_library(comm
            connection.cpp
            protocol.cpp
            httpconnection.cpp
            httpifcthread.cpp
            http-proto.cpp httpdomainspec.cpp
            httpsconnection.cpp
            httpsifcthread.cpp
            ifcthread.cpp
            none-json-proto.cpp
            jsonconnection.cpp
            jsonifcthread.cpp
            phpconnection.cpp
            phpifcthread.cpp
            phpsconnection.cpp
            phpsifcthread.cpp
            sslconnection.cpp
            sslifcthread.cpp
            udpifcthread.cpp
            tcpconnection.cpp
            wsconnection.cpp
            wsifcthread.cpp
            ws-json-proto.cpp
            wssconnection.cpp
            wssifcthread.cpp
            binifc.cpp binsifc.cpp
            fieldtoken.cpp
            pdu/httpresponse.cpp
            pdu/httprequest.cpp
            cx509.cpp)
