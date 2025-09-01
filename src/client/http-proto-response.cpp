//
// Created by ltoo on 04.04.24.
//
#include <iostream>
#include <cstring>
#include <cstdlib>
#include "http-proto-response.h"

namespace client {
    const char *version_match = "HTTP/1.1";

    char *HttpProtoResponse::process(char *a_buffer, int a_size) {
        char *s = a_buffer;
        //
        //  Run along the buffer. We use pointer arithmetic here as this
        //  is somewhat convenient.
        while ((m_state != 6) && (m_state != -1) && ((s - a_buffer) < a_size)) {
            switch (m_state) {
                case 0:  //  start of response. Probably.
                    if (strchr(version_match, *s) != nullptr) {
                        m_version.push_back(*s);
                    } else if (*s == ' ') {
                        //
                        //  Got the delimiter. Now check the version string itself.
                        //  If everything is good we go on.
                        if (m_version == version_match) {
                            m_state = 1;
                        } else {
                            std::cerr << "Http Response version string mismatch: " << m_version << std::endl;
                            m_state = -1;  //  Error indication.
                            return nullptr;
                        }
                    } else {
                        std::cerr << "Http Response invalid character in version string: " << m_version << "::" << *s
                                  << std::endl;
                        m_state = -1;  //  Error indication.
                        return nullptr;
                    }
                    break;
                case 1:
                    //
                    //  Even it should not happen. Maybe the server used more than a single space character to
                    //  separate version and response code. So skipping additional spaces here.
                    if ((*s != ' ') || (!m_response_code.empty())) {
                        //
                        // We only expect numbers.
                        if (isdigit(*s)) {
                            m_response_code.push_back(*s);
                        } else if (*s == ' ') {
                            //
                            //  Next is message it self.
                            m_state = 2;
                        } else {
                            std::cerr << "Http Response invalid character in version string: " << m_version << "::"
                                      << *s
                                      << std::endl;
                            m_state = -1;  //  Error indication.
                            return nullptr;
                        }
                    } else {
                        //
                        // ignore extra spaces.
                    }
                    break;
                case 2:
                    //
                    //  Even it should not happen. Maybe the server used more than a single space character to
                    //  separate response code and response message. So skipping additional spaces here.
                    if ((*s != ' ') || (!m_message.empty())) {
                        //
                        //  We read the message until the end-of-line marker 0x0d 0x0a (CRLF)
                        if ((*s != '\x0d') && (*s != '\x0a')) {
                            m_message.push_back(*s);
                        } else {
                            //
                            //  We ignore 0x0d and change the state on 0x0a
                            if (*s == '\x0a') {
                                m_state = 3;
                            }
                        }
                    } else {
                        //
                        // ignore extra spaces.
                    }
                    break;
                case 3:  //  Now the header fields start. Reading the header name.
                    if (*s != ':') {
                        //
                        //  Check for final 0x0d 0x0a
                        if ((*s != '\x0d') && (*s != '\x0a')) {
                            m_name.push_back(*s);
                        } else {
                            //
                            //  This should be the end of the header-fields. We may got a payload.
                            if (*s == '\x0a') {
                                //
                                //  Try to get a content length that tells us what size the payload is.
                                m_content_length = strtol(m_header_fields["Content-Length"].c_str(), nullptr, 0);

                                if (m_content_length > 0) {
                                    m_payload.reserve(m_content_length);
                                    m_state = 5;
                                } else {
                                    m_state = 6;
                                }
                            }
                        }
                    } else {
                        //
                        //  Before moving on we trim the m_name, removing trailing spaces.
                        while ((m_name.size() > 0) && (m_name[m_name.size() - 1] == ' ')) {
                            m_name.erase(m_name.size() - 1);
                        }
                        m_state = 4;
                    }
                    break;
                case 4 :  //  read the header field value until the end of the line.
                    //
                    //  We read the headerfield content until the end-of-line marker 0x0d 0x0a (CRLF)
                    if ((*s != '\x0d') && (*s != '\x0a')) {
                        m_value.push_back(*s);
                    } else {
                        //
                        //  We ignore 0x0d and store name/value on 0x0a. Changing state back to read
                        //  the next header field name
                        if (*s == '\x0a') {
                            m_header_fields.emplace(m_name, m_value);
                            m_name.clear();
                            m_value.clear();
                            m_state = 3;
                        }
                    }

                    break;
                case 5:  // read the payload.
                    m_payload.push_back(*s);
                    if (m_payload.size() >= (size_t)m_content_length) {
                        m_state = 6; // We are done.
                    }
                case 6:
                    break;
                default:
                    std::cerr << "Invalid http response parser state\n";
                    break;
            }
            s++;
        }
        //
        //  Check if we have processed the whole buffer.
        if ((s - a_buffer) >= a_size) {
            s = nullptr;
        }
        return s;
    }


}