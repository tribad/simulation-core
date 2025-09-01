#include <cstdlib>
#include <cstring>
#include <iostream>
#include "json.h"

static int indentation=0;

static std::ostream & dump(std::ostream &output, tJSONArray *array);

inline static void parser_init(tJSON *p) {
    p->state  = 0;
    p->parent = 0;
    p->temp   = 0;
}
//
//  These conversion functions are used in the parser and do return
//  zero in error to allow direct use in calculations. 
//  This way we do not need locals

static uint8_t char2oct(char c) {
    uint8_t retval = 0;

    if (isdigit(c) && (c < '8')) {
        retval = c - '0';
    }
    return retval;
}

static uint64_t char2hex(char c) {
    uint64_t retval = 0;

    if (isxdigit(c)) {
        retval = c - '0';
        if (retval > 9) {
            retval = (c - 'A') + 10;
            if (retval > 15) {
                retval = (c -  'a') + 10;
            }
        }
    }
    return retval;
}
//
//  End of special converters.


tJSON* parse(std::istream &infile) {
    tJSON* retval = 0;
    char   c;

    while (infile.good()) {
        infile >> c;

        retval=(tJSON*)parse(retval, (uint8_t*)(&c), 1);
        if ((retval != nullptr) && (completed(retval))) {
            break;
        }
    }
    while ((retval != nullptr) && (retval->parent != nullptr)) {
        retval = retval->parent;
    }
    return (retval);
}

tJSON *parse(tJSON *prev, uint8_t*data, size_t size) {
    size_t      index=0;
    bool        again=false;
    tJSON       *root;
    tJSONValue  *jnewvalue;
    tJSONObject *jnewobject;
    tJSONArray  *jnewarray;
    uint8_t     c;

    if (prev==0) {
        root=prev=(tJSON*)(new tJSONObject);
        root->type=eObject;
        parser_init(prev);
    } else {
        root=prev;
        while (root->parent!=0) {
            root=root->parent;
        }
    }

    for (index=0; index<size;) {
        c=data[index];
        switch (root->state) {
        case 0:    // init
            if (c == '{') {
                root->state=1; // we expect a namestart
            }
            break;
        case 1:   // name start
            if (c == '"') {
                root->state=2; // we now take the name
                root->name.clear();
            } else if (c=='}') {
                root->state=-1;
            }
            break;
        case 2:    //   the name literal
            if (c !='"') {
                root->name.push_back((char)(c));
            } else {
                root->state = 3; // Wait colon
            }
            break;
        case 3:    //  Wait colon
            if (c == ':') {
                root->state = 4;  //  Wait next non blank. This tells us what type of object we have to expect.
            }
            break;
        case 4:
            if (!isblank(c)) {
                if (c=='"') {
                    jnewvalue = new tJSONValue(root->name, prev);
                    root->name.clear();
                    prev        = jnewvalue;
                    root->state = 199;               //  Wait for end of string
                } else if (toupper(c)=='T') {
                    jnewvalue = new tJSONValue(root->name, prev);
                    root->name.clear();
                    prev             = jnewvalue;
                    jnewvalue->value = true;
                    root->state      = 200;               //  Wait for end of value
                } else if (toupper(c)=='F') {
                    jnewvalue = new tJSONValue(root->name, prev);
                    root->name.clear();
                    prev             = jnewvalue;
                    jnewvalue->value = false;
                    root->state      = 200;               //  Wait for end of value
                } else if (toupper(c)=='N') {
                    jnewvalue = new tJSONValue (root->name, prev);
                    root->name.clear();
                    prev        = jnewvalue;
                    root->state = 200;               //  Wait for end of value
                } else if (c=='{') {
                    jnewobject = new tJSONObject(root->name, prev);
                    root->name.clear();
                    prev        = jnewobject;
                    root->state = 1;
                } else if (c=='[') {                   //  Process array
                    jnewarray = new tJSONArray(root->name, prev);
                    root->name.clear();
                    prev = jnewarray;
                } else if (isdigit(c) || (c=='+') || (c=='-')) {
                    jnewvalue = new tJSONValue(root->name, prev);
                    root->name.clear();
                    root->name.push_back(c);
                    prev             = jnewvalue;
                    jnewvalue->value = 0;
                    root->state      = 197;                        //  Process numeric
                } else if (c==']') {
                    root->state = 200;
                }
            }
            break;
        case 170:   // processing first char of hex value
            root->temp = char2hex(c) << 4;
            root->state = 171;
            break;
        case 171:   // processing second char of hex value
            root->temp += char2hex(c);
            ((tJSONValue*)prev)->value.push_back(root->temp);   
            root->state = 199;
            break;
        case 180:   // processing first char of octal value
            root->temp = char2oct(c) << 6;
            root->state = 181;
            break;
        case 181:   // processing second char of octal value
            root->temp += char2oct(c) << 3;
            root->state = 182;
            break;
        case 182:   // processing third char of octal value
            root->temp += char2oct(c);
            ((tJSONValue*)prev)->value.push_back(root->temp);   
            root->state = 199;
            break;
        case 193:   //  processing of char one of a 2-byte unicode
            root->temp = char2hex(c) << 12;
            root->state = 194;
            break;
        case 194:   //  processing of char two a 2-byte unicode
            root->temp += char2hex(c) << 8;
            root->state = 195;
            break;
        case 195:   //  processing of char three a 2-byte unicode
            root->temp += char2hex(c) << 4;
            root->state = 196;
            break;
        case 196:   //  processing of char for of a 2-byte unicode
            {
                char mb[MB_CUR_MAX];
                int  mbsize;
          
                root->temp += char2hex(c);
                std::wctomb(nullptr, 0);
                mbsize = std::wctomb(mb, (uint16_t)(root->temp));
                if (mbsize > 0) {
                    //
                    //  as the variant only knows push_back we push the charcters
                    //  this is no problem as we only do that in minor cases.
                    for (int i = 0; i < mbsize; ++i) {
                        ((tJSONValue*)prev)->value.push_back(mb[i]);   
                    }
                } else {
		    std::cerr << "Connot convert UNICODE char\n";
		}
                root->state = 199;
            }
            break;
        case 197:   //  processing numeric
            if ((c==',') || (c=='}') || (c == ']')) {
                tJSONValue* pv=(tJSONValue*)prev;

                if ((*pv) == eVariant::Null) {
                    pv->value = strtoll(root->name.c_str(), 0, 10);
                } else if ((*pv)==eVariant::Int) {
                    pv->value = strtoll(root->name.c_str(), 0, 10);
                } else {
                    pv->value = strtold(root->name.c_str(), 0);
                }
                again=true;
                root->state=200;
                root->name.clear();
            } else if (((toupper(c)=='E') || (c=='.')) && (*((tJSONValue*)prev) == eVariant::Null)) {
                root->name.push_back(c);
            } else if ((c==' ') && (*((tJSONValue*)prev) == eVariant::Null)) {
            } else {
                root->name.push_back(c);
            }
            break;
        case 198:   // in escape sequence
            //
            // The default follow-up state is the 199.
            // can be changed for special formats.
            root->state = 199;
            if (c=='\\') {
                ((tJSONValue*)prev)->value.push_back('\\');
            } else {
                if (c=='n') {
                    ((tJSONValue*)prev)->value.push_back('\n');
                } else if (c=='t') {
                    ((tJSONValue*)prev)->value.push_back('\t');
                } else if (c=='"') {
                    ((tJSONValue*)prev)->value.push_back('"');
                } else if (c=='r') {
                    ((tJSONValue*)prev)->value.push_back('\r');
                } else if (c=='b') {
                    ((tJSONValue*)prev)->value.push_back('\b');
                } else if (c=='0') {
                    root->temp = 0u;
                    root->state = 180;           //  octal value
                } else if (c=='x') {
                    root->temp = 0u;
                    root->state = 170;           // hex value
                } else if (c== 'u') {            // unicode ... need conversion.
                    root->temp = 0u;
                    root->state = 193;
                } else {
                }
            }
            break;
        case 199:                   //  Wait for end of string
            if (c=='\\') {
                root->state = 198;
            } else if (c=='"') {
                root->state = 200;  //  Wait end of value
            } else {
                ((tJSONValue*)prev)->value.push_back(c);
            }
            break;
        case 200:
            if ((c==',') || (c=='}') || (c==']')) {
                if (prev->parent != nullptr) {
                    if (prev->parent->type==eObject) {
                        ((tJSONObject*)prev->parent)->values.insert(std::pair<std::string, tJSON*>(prev->name, prev));
                        if (c==',') {
                            root->state = 1;      //  Continue with a name
                        } else if (c=='}') {
                            //
                            //  Make the parent to the actual object.
                            prev = prev->parent;
                            if ((prev->parent != nullptr) && (prev->parent->type == eObject)) {
                                ((tJSONObject*)prev->parent)->values.insert(std::pair<std::string, tJSON*>(prev->name, prev));
                                root->state = 1;      //  Continue with a name
                            } else if ((prev->parent != nullptr) && (prev->parent->type == eArray)) {
                                ((tJSONArray*)prev->parent)->values.push_back(prev);
                                root->state = 4;      //  Continue with a value;
                            } else if (prev->parent == nullptr) {
                                root->state = -1;
                            }
                        }
                    } else if (prev->parent->type==eArray) {
                        //
                        //  Put the value into the parent, that is the array object.
                        ((tJSONArray*)prev->parent)->values.push_back(prev);
                        if (c==',') {
                            root->state = 4;      //  Continue with a value;
                        } else if (c == ']') {
                            //
                            //  Move prev one up. That is the array object it self.
                            //
                            //  No. NO. We are moving the pointer some lines later. So we must not do it here.
                            //  Code stays here to warn of this idea.
                            ///prev = prev->parent;
                            //
                            //  Do not need to do more here.
                        } else {
                        }
                    }
                    if (prev->parent != nullptr) {
                        prev = prev->parent;
                        //
                        //  We are now on the root object.
                        if (prev->parent == nullptr) {
                            //
                            // If it was the closing brace on the root object it is completed.
                            if (c=='}') {
                                root->state=-1;
                            }
                        }
                    }
                } else {
                    if (c=='}') {
                        root->state=-1;
                    }
                }
            }
            break;
        default:
            break;
        }
        if (again) {
            again=false;
        } else {
            index++;
        }
    }
    return (prev);
}

static std::ostream & dump(std::ostream &output, tVariant *value) {
    std::string v = (std::string)(*value);
    if (!value->valid()) {
        output << "<null>";
    } else if (v.empty()) {
        output << "<empty-string>";
    } else {
        output << v;
    }
    return (output);
}
static std::ostream & dump(std::ostream &output, tJSONObject *obj) {
    std::map<std::string, tJSON*>::iterator i;
    bool delimiter=false;

    if (obj->name.size() > 0) {
        output << obj->name << " {\n";
    } else {
        output << "{\n";
    }
    indentation+=4;
    for (i=obj->values.begin(); i!=obj->values.end(); ++i) {
        if (delimiter) {
            output << ",\n";
        } else {
            delimiter=true;
        }
        for (int x=0;x<indentation;x++) output << ' ';
        output << "\"" << i->first << "\" : ";
        switch(i->second->type) {
        case eValue:
            dump(output, &((tJSONValue*)(i->second))->value);
            break;
        case eArray:
            dump(output, ((tJSONArray*)(i->second)));
            break;
        case eObject:
            dump(output, (tJSONObject*)i->second);
            delimiter=false;
            break;
        default:
            break;
        }
    }
    indentation-=4;
    output << "\n";
    for (int x=0;x<indentation;x++) output << ' ';
    output << "}\n";
    return (output);
}


static std::ostream & dump(std::ostream &output, tJSONArray *array) {
    std::vector<tJSON*>::iterator i;
    output << "[ ";

    for (i=array->values.begin(); i!=array->values.end(); ++i) {
        if (i!=array->values.begin()) {
            output << ", ";
        }
        switch ((*i)->type) {
        case eValue:
            dump(output, &((tJSONValue*)(*i))->value);
            break;
        case eArray:
            dump(output, ((tJSONArray*)(*i)));
            break;
        case eObject:
            dump(output, (tJSONObject*)(*i));
            break;
        default:
            break;
        }
    }
    output << " ]";
    return (output);
}



std::ostream & dump(std::ostream & output, tJSON *root) {
    indentation=0;
//    while (root->parent != 0) root=root->parent;
    return (dump(output, (tJSONObject*)root));
}


tJSON *find(tJSON *root, std::string path) {
    size_t end;
    std::map<std::string, tJSON*>::iterator i;

    if (root->type==eObject) {
        end=path.find_first_of('/');
        tJSONObject *o=(tJSONObject*)(root);
        std::string s=path.substr(0, end);
        i=o->values.find(s);
        if (i!=o->values.end()) {
            return (i->second);
        } else {
        }
    } else if (root->type==eArray) {
        end=path.find_first_of(':');
    } else {
        return (root);
    }
    return (0);
}

int to_int(tJSON *j) {
    return ((tJSONValue*)j)->value;
}

unsigned long to_unsigned_long(tJSON *j) {
    return ((((tJSONValue*)j)->value));
}


long to_long(tJSON *j) {
    return ((long)(((tJSONValue*)j)->value));
}

int64_t to_int64_t(tJSON *j) {
    return ((long)(((tJSONValue*)j)->value));
}

uint8_t to_uint8_t(tJSON *j) {
    uint64_t retval = (((tJSONValue*)j)->value);

    return (uint8_t)retval;
}

uint16_t to_uint16_t(tJSON *j) {
    return (((tJSONValue*)j)->value);
}

uint64_t to_uint64_t(tJSON *j) {
    return (((tJSONValue*)j)->value);
}

int16_t to_int16_t(tJSON *j) {
    return (((tJSONValue*)j)->value);
}

bool to_bool(tJSON* j) {
    return ((bool)(((tJSONValue*)j)->value));
}

std::string to_string(tJSON *j) {
    return ((std::string)(((tJSONValue*)j)->value));
}

double to_double (tJSON* j) {
    return ((double)(((tJSONValue*)j)->value));
}

bool completed(tJSON *root) {
    if (root != nullptr) {
        return (root->state == -1);
    }
    return (false);
}

#ifdef TEST
#include <stdio.h>

FILE *infile=stdin;

int main(int argc, char **argv) {
    uint8_t buffer[512];
    size_t dataread;
    tJSON *pp=0;

    if (argc > 1) {
        infile=fopen(argv[1], "r+t");
    }
    if (infile != 0) {
        while (!feof(infile)) {
            dataread=fread(buffer, 1, 512, infile);
            pp=parse(pp, buffer, dataread);
        }
        dump(std::cout, pp);
    } else {
    }
    return (0);
}

#endif
