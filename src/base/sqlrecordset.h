// *************************************************************************************************************
//
//  Modul-Name     : sqlrecordset.h
//
//  Copyrights 2016 by Hans-Juergen Lange <hjl@simulated-universe.de>. All rights reserved.
//
// *************************************************************************************************************
// HOC *********************************************************************************************************
//          |         |
// YYYYMMDD | Creator | description
//--------------------------------------------------------------------------------------------------------------
//          |         |
//----------+---------+-----------------------------------------------------------------------------------------
//          |         |
// HOC end *****************************************************************************************************
#pragma once
#ifndef SQLRECORDSET_INC
#define SQLRECORDSET_INC

#include <string>
#include <vector>
#include <variant.h>

struct tRecord {
    tRecord() {}
    ~tRecord() {}
    const tVariant& operator[](int aNr) const
    {
        static tVariant emptyField;

        if ((size_t)(aNr)<Fields.size()) {
            return Fields[aNr];
        } else {
            return emptyField;
        }
    }
    uint32_t              Number;
    std::vector<tVariant> Fields;
};

struct tSQLRecordSet {
    tSQLRecordSet() {Error = 0; Count = 0; FieldCount = 0;}
    ~tSQLRecordSet() {}
    const tRecord& operator[](uint32_t aNr)
    {
        static tRecord nullRecord;

        if (aNr < Records.size()) {
            return Records[aNr];
        } else {
            return nullRecord;
        }
    }
    int                        Error;
    uint32_t                   Count;
    uint32_t                   FieldCount;
    std::vector <std::string>  FieldNames;
    std::vector <tRecord>      Records;
};

#endif // SQLRECORDSET_INC
