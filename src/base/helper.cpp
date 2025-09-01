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

#include <cctype>
#include <algorithm>
#include <cstring>
#include "helper.h"

std::string helper::tolower(const std::string &text) {
    std::string lower;

    lower.resize(text.size());
    std::transform(text.begin(), text.end(), lower.begin(), ::tolower);

    return lower;
}

std::string helper::toupper(const std::string &text) {
    std::string upper;

    upper.resize(text.size());
    std::transform(text.begin(), text.end(), upper.begin(), ::toupper);

    return upper;
}

std::string helper::normalize(const std::string &text) {
    std::string norm;
    std::string to_be_replaced("_.,;:/() -");

    for (size_t i=0; i<text.size(); ++i) {
        if (to_be_replaced.find_first_of(text[i]) != std::string::npos) {
            norm.push_back('_');
        } else {
            norm.push_back(text[i]);
        }
    }


    return norm;
}

std::string helper::escapeHTML(const std::string &text) {
    std::string retval = text;
    size_t position;

    while ((position = retval.find_first_of("<\"'")) != std::string::npos) {
        switch (retval[position]) {
        case '<':
            retval.replace(position, 1, "&lt;");
            break;
        case '"':
            retval.replace(position, 1, "&quot;");
            break;
        case '\'':
            retval.replace(position, 1, "&apos;");
            break;
        default:
            return retval;
        }
    }
    return retval;
}

std::string helper::escape(const std::string &text) {
    size_t      pos;
    std::string escaped;
    std::string to_be_replaced("\n\t\\\"\b\r");

    for (size_t i=0; i<text.size(); ++i) {
        pos = to_be_replaced.find_first_of(text[i]);
        if ( pos!= std::string::npos) {
            switch (text[i]) {
            case '\n':
                escaped.push_back('\\');
                escaped.push_back('n');
                break;
            case '\t':
                escaped.push_back('\\');
                escaped.push_back('t');
                break;
            case '\\':
                escaped.push_back('\\');
                escaped.push_back('\\');
                break;
            case '"':
                escaped.push_back('\\');
                escaped.push_back('"');
                break;
            case '\r':
                escaped.push_back('\\');
                escaped.push_back('r');
                break;
            case '\b':
                escaped.push_back('\\');
                escaped.push_back('b');
                break;
            default:
                escaped.push_back(text[i]);
                break;
            }
        } else {
            escaped.push_back(text[i]);
        }
    }


    return escaped;
}

std::list<std::pair<std::string, std::string> > helper::typelist(std::string cname) {
    int         state     = 0;
    int         paralevel = 0;
    std::string ident;
    std::string ns;
    std::string typ;
    std::list<std::pair<std::string, std::string> > retval;

    for (std::string::iterator ci=cname.begin(); ci != cname.end(); ++ci) {
        switch (state) {
        case 0:     // start of some item must start with a alpha.
            if (isalpha(*ci)) {
                state = 1;
                ident.push_back(*ci);
            }
            break;
        case 1:     //  now wait until end. can be any alnum
            if ((isalnum(*ci)) || ((*ci)== '*') || ((*ci) == '&') || ((*ci) == '_')) {
                ident.push_back(*ci);
            } else {
                if ((*ci)==':') {
                    ns = ident;
                    ident.clear();
                    state = 2;      //  Wait for other colon
                } else if ((*ci) == '<') {   //  Template parameter follow. Starting with state 0.
                    paralevel++;
                    state = 0;
                    typ   = ident;
                    ident.clear();

                    retval.push_back(std::pair<std::string, std::string>(ns, typ));
                    ns.clear();
                } else if ((*ci)==',') {  //  More parameter to templates.
                    state = 0;
                    typ = ident;
                    ident.clear();

                    retval.push_back(std::pair<std::string, std::string>(ns, typ));
                    ns.clear();
                } else if ((*ci) == '>') {
                    paralevel --;
                    state = 0;
                    typ   = ident;
                    ident.clear();

                    retval.push_back(std::pair<std::string, std::string>(ns, typ));
                    ns.clear();
                }

            }
            break;
        case 2:     //  Wait for second colon.
            if ((*ci) == ':') {
                state = 3;  //  the typename follows.
            } else {
                return retval;   //  Abort because of syntax error.
            }
            break;
        case 3:
            if (isalpha(*ci)) {
                ident.push_back(*ci);
                state = 4;   // more typename
            } else {
            }
            break;
        case 4:     //  More typename.
            if ((isalnum(*ci)) || ((*ci)== '*') || ((*ci) == '&') || ((*ci) == '_')) {
                ident.push_back(*ci);
            } else {
                if ((*ci) == '<') {   //  Template parameter follow. Starting with state 0.
                    paralevel++;
                    state = 0;
                    typ   = ident;
                    ident.clear();

                    retval.push_back(std::pair<std::string, std::string>(ns, typ));
                    ns.clear();
                } else if ((*ci)==',') {  //  More parameter to templates.
                    state = 0;
                    typ = ident;
                    ident.clear();

                    retval.push_back(std::pair<std::string, std::string>(ns, typ));
                    ns.clear();
                } else if ((*ci) == '>') {
                    paralevel --;
                    state = 0;
                    typ   = ident;
                    ident.clear();

                    retval.push_back(std::pair<std::string, std::string>(ns, typ));
                    ns.clear();
                }
            }
            break;
        default:
            break;
        }
    }
    if ((state == 4) || (state == 1)) {
        retval.push_back(std::pair<std::string, std::string>(ns, ident));
    }
    return retval;
}

std::list<std::string> helper::tokenize(std::string line, const char* delimiter) {
    std::list<std::string> retval;
    size_t                 start = 0;
    size_t                 lpos;

    while ((lpos = line.find_first_of(delimiter, start)) != std::string::npos) {
        std::string token = line.substr(start, lpos-start);

        token = trim(token);
        if (!token.empty()) {
            retval.push_back(token);
        }

        start = lpos+1;
    }
    if (start < line.size()) {
        std::string token = line.substr(start);

        token = trim(token);
        if (!token.empty()) {
            retval.push_back(token);
        }
    }

    return retval;
}

std::string helper::trim(const std::string& line) {
    std::string retval;
// User-Defined-Code:AAAAAAFxaDbcJg+TZEk=
    retval = line;
    while ((retval.size()>0) && (isspace(retval[0]))) {
        retval.erase(0,1);
    }
    while ((retval.size() > 0) && (isspace(retval[retval.size()-1]))) {
        retval.erase(retval.size()-1);
    }
// End-Of-UDC:AAAAAAFxaDbcJg+TZEk=
    return  (retval);
}

std::string helper::xml_string_property(const xmlNode *node, std::string name) {
    std::string retval;

    if (node != 0) {
#if (LIBXML_VERSION >=20907)
        xmlChar* prop = xmlGetProp(node, (const xmlChar*)name.c_str());
#else
        xmlChar* prop = xmlGetProp((xmlNodePtr)node, (const xmlChar*)name.c_str());
#endif
        if (prop != 0) {
            retval = (const char*)prop;

            xmlFree(prop);
        }
    }
    return retval;
}
long helper::xml_long_property(const xmlNode *node, std::string name, long defValue) {
    long retval=defValue;

    if (node != 0) {
#if (LIBXML_VERSION >=20907)
        xmlChar* prop = xmlGetProp(node, (const xmlChar*)name.c_str());
#else
        xmlChar* prop = xmlGetProp((xmlNodePtr)node, (const xmlChar*)name.c_str());
#endif
        if (prop != 0) {
            retval = strtol((const char*)prop, 0, 0);

            xmlFree(prop);
        }
    }
    return retval;
}
bool helper::xml_bool_property(const xmlNode *node, std::string name, bool defValue) {
    bool retval = defValue;

    if (node != 0) {
#if (LIBXML_VERSION >=20907)
        xmlChar* prop = xmlGetProp(node, (const xmlChar*)name.c_str());
#else
        xmlChar* prop = xmlGetProp((xmlNodePtr)node, (const xmlChar*)name.c_str());
#endif
        if (prop != 0) {
            if (strcmp((const char*)prop, "true")==0) {
                retval = true;
            }
            xmlFree(prop);
        }
    }
    return retval;
}

xmlNode* helper::xml_find_child(xmlNode *node, std::string name) {
    xmlNode* children = node->children;

    while (children != 0) {
        if (children->type == XML_ELEMENT_NODE) {
            if (helper::tolower(std::string((const char*)(children->name))) == name) {
                return children;
            }
        }
        children = children->next;
    }
    return 0;
}

std::string helper::xml_content(xmlNode *node) {
    std::string retval;
    xmlNode* children = node->children;

    while (children != 0) {
        if (children->type == XML_TEXT_NODE) {
            retval = (const char*)(children->content);
            break;
        }
        children = children->next;
    }
    return retval;
}
