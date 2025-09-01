// MIT License
//
// Copyright (c) 2017 Hans-Juergen Lange<hjl@simulated-universe.de>
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

#ifndef HELPER_H
#define HELPER_H

#include <string>
#include <list>
#include <libxml/parser.h>
#include <libxml/tree.h>

class helper
{
public:
    helper() = default;
    static std::string toupper(const std::string& text);
    static std::string tolower(const std::string& text);
    static std::string normalize(const std::string& text);
    static std::string escape(const std::string& text);
    static std::string escapeHTML(const std::string & text);
    static std::list<std::pair<std::string, std::string> > typelist(std::string cname);
    static std::list<std::string> tokenize(std::string line, const char* delimiter=" \t");
    static std::string trim(const std::string& line);
    static std::string xml_string_property(const xmlNode* node, std::string name);
    static long xml_long_property(const xmlNode* node, std::string name, long defValue = 0);
    static bool xml_bool_property(const xmlNode* node, std::string name, bool defValue = false);
    static xmlNode* xml_find_child(xmlNode* node, std::string name);
    static std::string xml_content(xmlNode* node);
};

#endif // HELPER_H
