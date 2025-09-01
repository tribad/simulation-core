#ifndef JSON_H
#define JSON_H

#include <stdint.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <variant.h>

enum eJSON {
    eValue=0x0001,
    eObject,
    eArray,
    eInvalid
};

struct tJSON {
    tJSON() = default;
    tJSON(std::string aName, tJSON* aParent) : parent(aParent), name(aName) {};
    virtual ~tJSON()  = default;

    tJSON*         parent = nullptr;
    int16_t        state  = 0;
    uint32_t       temp   = 0;
    eJSON          type   = eInvalid;
    std::string    name;
};

struct tJSONValue : public tJSON {
    tJSONValue() { type = eValue; }
    tJSONValue(std::string aName, tJSON* aParent) : tJSON(aName, aParent) {type = eValue;}
    virtual ~tJSONValue() = default;
    tVariant value;
    operator std::string () {return (std::string)value;}
    bool operator==(eVariant vmatch) {return value.isType(vmatch);}
};

struct tJSONArray  : public tJSON {
    tJSONArray() = default;
    tJSONArray(std::string aName, tJSON* aParent) : tJSON(aName, aParent) {type = eArray;}
    virtual ~tJSONArray() {
         for (auto v : values) {
             if (v != nullptr) {
		    delete v;
	     }
	 }
    }
    std::vector<tJSON*> values;
};

struct tJSONObject   : public tJSON {
    tJSONObject() = default;
    tJSONObject(std::string aName, tJSON* aParent) : tJSON(aName, aParent) {type = eObject;}
    virtual ~tJSONObject() {
	    for (auto o : values) {
		    if (o.second != nullptr) {
			    delete o.second;
		    }
	    }
    }
    std::map<std::string, tJSON*> values;
};

struct tJSONParser {
    ~tJSONParser();
    int   state;
    tJSON *root;
    tJSON *now;
};

tJSON *parse(tJSON *prev, uint8_t*data, size_t size) ;
tJSON* parse(std::istream& infile);

std::ostream & dump(std::ostream & output, tJSON *root) ;
tJSON *find(tJSON *root, std::string path);
bool completed(tJSON* root);


double        to_double(tJSON*);
int           to_int(tJSON*);
unsigned long to_unsigned_long(tJSON *j) ;
uint8_t       to_uint8_t(tJSON* j);
uint16_t      to_uint16_t(tJSON* j);
uint64_t      to_uint64_t(tJSON* j);
int16_t       to_int16_t(tJSON* j);
int64_t       to_int64_t(tJSON*);
std::string   to_string(tJSON*);
bool          to_bool(tJSON*);
long          to_long(tJSON*);

#endif // JSON_H

