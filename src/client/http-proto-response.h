// MIT License
//
// Copyright (c) 2024 Hans-Juergen Lange<hjl@simulated-universe.de>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


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
