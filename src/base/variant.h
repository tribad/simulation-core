// MIT License
//
// Copyright (c) 2015 Hans-Juergen Lange<hjl@simulated-universe.de>
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

#ifndef VARIANT_INC
#define VARIANT_INC

#include <stdint.h>
#include <string>
#include <cstring>
#include <malloc.h>
#include <ctime>

enum class eVariant {
    Double,
    Int,
    Unsigned,
    Reference,
    String,
    Boolean,
    Null
};

class tVariant {
public:
#if defined(APPCORE_BASE_VARIANT_TEST_FRIEND)
    APPCORE_BASE_VARIANT_TEST_FRIEND();
#endif
    tVariant() {type = eVariant::Null; ul = 0;}
    tVariant(const tVariant & in);
    tVariant(const std::string& s) : tVariant(s.c_str()) {}
    tVariant(const char* t);
    tVariant(uint8_t       u) {type = eVariant::Unsigned; ul = u;}
    tVariant(uint16_t      u) {type = eVariant::Unsigned; ul = u;}
    tVariant(uint32_t      u) {type = eVariant::Unsigned; ul = u;}
    tVariant(uint64_t      u) {type = eVariant::Unsigned; ul = u;}
    tVariant(int8_t        i) {type = eVariant::Int;      il = i;}
    tVariant(int16_t       i) {type = eVariant::Int;      il = i;}
    tVariant(int32_t       i) {type = eVariant::Int;      il = i;}
    tVariant(int64_t       i) {type = eVariant::Int;      il = i;}
    tVariant(long long int i) {type = eVariant::Int;      il = i;}
    tVariant(double        d) {type = eVariant::Double; dbl = d;}
    tVariant(long double   d) {type = eVariant::Double; dbl = d;}
    tVariant(void*         p) {type = eVariant::Reference; ptr = p;}

    tVariant(bool b) {type =eVariant::Boolean; ul = b;}

    ~tVariant() {if ((type == eVariant::String) && (ptr != nullptr)) {free(ptr);ptr=nullptr;}}
    bool isType(eVariant t) const {return type == t;}

    void clear() {
        if ((type == eVariant::String) && (ptr != nullptr)) {
            free(ptr);
            ptr = nullptr;
        }
        type = eVariant::Null;
    }

    bool valid() {return (type != eVariant::Null);}

    operator unsigned long() const ;
    operator uint8_t() const {return ((unsigned long) (*this));}
    operator uint16_t() const {return ((unsigned long) (*this));}
    operator uint32_t() const {return ((unsigned long) (*this));}


    operator long double () const;
    operator double () const {return ((long double)(*this));}
    operator float () const {return ((long double)(*this));}

    operator long () const;
    operator int () const { return ((long)(*this));}
    operator int8_t () const { return ((long)(*this));}
    operator int16_t () const { return ((long)(*this));}

    operator bool() const;
    operator std::string() const;

    operator void*() const;

    const tVariant& operator = (const tVariant& in);

    bool operator==(const bool value) const {return ((bool)(*this))==value;}
    bool operator==(const double value) const {return ((double)(*this))==value;}
    bool operator==(const int value) const {return ((int)(*this)) == value;}
    bool operator==(const long value) const {return ((long)(*this))==value;}
    bool operator==(const unsigned long value) const {return ((unsigned long)(*this))==value;}
    bool operator==(const std::string& thingie) const {return (const std::string)(*this) == thingie;}
    bool operator==(const tVariant& v) const;
    bool operator==(const char*value) const {
            if (value != nullptr) {
                return this->operator std::string() == value;
            } else {
                return false;
            }
        }
    bool operator==(void* value) const {return (const void*)(*this) == value;}


    bool operator!=(long unsigned int value) const {return (long unsigned int)(*this) != value;}
    bool operator!=(long value) const {return (long)(*this) != value;}
    bool operator!=(int value) const {return (int)(*this) != value;}
    bool operator!=(const char*value) const {
        if (value != nullptr) {
            return this->operator std::string() != value;
        } else {
            return false;
        }
    }
    bool operator!=(void* value) const {return (void*)(*this) != value;}

    bool operator!=(const std::string& value) const {return this->operator std::string() != value;}
    bool operator!=(bool value) const {return (bool)(*this) != value;}
    bool operator!=(const tVariant& value) const;

    unsigned int operator& (unsigned int value) {return (long unsigned)(*this) & value;}
    unsigned long operator|= (unsigned int value) {return (*this) = ((long unsigned)(*this) | value);}

    tVariant & push_back(char c);
    bool empty() {return this->operator std::string().empty();}

    int operator+(int value) {return ((long)(*this))+value;}
    unsigned long operator+(unsigned long value) {return ((unsigned long)(*this))+value;}
    int operator-(int value) {return ((long)(*this))-value;}
    unsigned long operator-(const unsigned long  value) {return ((unsigned long)(*this))-value;}
    int operator/(int value) {return ((long)(*this))/value;}
    int operator*(int value) {return ((long)(*this))*value;}
    unsigned long operator*(unsigned long value) {return ((unsigned long)(*this))*value;}
    tVariant operator++();
    tVariant operator++(int);
    tVariant operator--();
    tVariant operator--(int);


protected:
    eVariant        type = eVariant::Null;
    union {
        uint64_t    ul;
        int64_t     il;
        void*       ptr = nullptr;
        long double dbl;
    };

};
uint64_t operator-(uint64_t a, const tVariant& value);
bool operator>(uint64_t a, const tVariant& value);
bool operator<(const tVariant& value, uint64_t);
tVariant operator++(const tVariant& value);

#endif
