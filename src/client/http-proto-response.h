//
// Created by ltoo on 04.04.24.
//

#ifndef WSCLIENT_HTTP_PROTO_RESPONSE_H
#define WSCLIENT_HTTP_PROTO_RESPONSE_H
#include <string>
#include <map>

namespace client {

//
//  This class encapsulates the statemachine for processing a stream
// of bytes that may contain a http response.
    class HttpProtoResponse {
    public:
        HttpProtoResponse() = default;
        ~HttpProtoResponse() = default;
        char *process(char *a_buffer, int a_size);
        int get_state() { return m_state; }
        const std::string &response_code() { return m_response_code; }
    private:
        int                                m_state = 0;
        long                               m_content_length = 0;
        std::string                        m_version;
        std::string                        m_response_code;
        std::string                        m_message;
        std::string                        m_name;
        std::string                        m_value;
        std::string                        m_payload;
        std::map<std::string, std::string> m_header_fields;
    };

}

#endif //WSCLIENT_HTTP_PROTO_RESPONSE_H
