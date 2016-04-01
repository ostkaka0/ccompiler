#pragma once

#include <cstring>

struct Datatype;

enum Datatype_Type {
    TYPE_UNEVALUATED = 0,
    TYPE_VOID,
    
    // Types without parameters
    TYPE_SIZE_T,
    TYPE_UINT,
    TYPE_INT,
    TYPE_U8,
    TYPE_S8,
    TYPE_U16,
    TYPE_S16,
    TYPE_U32,
    TYPE_S32,
    TYPE_U64,
    TYPE_S64,
    TYPE_FLOAT32,
    TYPE_FLOAT64,
    
    // Types with parameters
    TYPE_PTR, // Pointer
    TYPE_PTR_OWNED,
    TYPE_STRUCT
};

union DatatypeParameter {
    Datatype* _ptr;
    //StructDef* _struct;
};

struct Datatype {
    DatatypeParameter parameter;
    Datatype_Type type = TYPE_UNEVALUATED;
};

static Datatype createDatatype(Datatype_Type type) {
    Datatype datatype;
    datatype.type = type;
    return datatype;
}

static Datatype createDatatype(const char* name) {
    size_t length = strlen(name);
    
    if (strcmp(name, "void") == 0)
        return createDatatype(TYPE_VOID);
    if (strcmp(name, "int") == 0)
        return createDatatype(TYPE_INT);
    if (strcmp(name, "char") == 0)
        return createDatatype(TYPE_S8);
    if (strcmp(name, "short") == 0)
        return createDatatype(TYPE_S16);
    if (strcmp(name, "long") == 0)
        return createDatatype(TYPE_S32);
    if (strcmp(name, "float") == 0)
        return createDatatype(TYPE_FLOAT32);
    if (strcmp(name, "double") == 0)
        return createDatatype(TYPE_FLOAT64);
    
    return createDatatype(TYPE_UNEVALUATED);
}

static Datatype createDatatypePtr(Datatype* datatypePtr) {
    Datatype datatype;
    datatype.type = TYPE_PTR;
    datatype.parameter._ptr = datatypePtr;
}

static Datatype createDatatypePtrOwned(Datatype* datatypePtr) {
    Datatype datatype;
    datatype.type = TYPE_PTR_OWNED;
    datatype.parameter._ptr = datatypePtr;
}

bool operator==(const Datatype& a, const Datatype& b) {
    if (a.type != b.type)
        return false;
    
    switch (a.type) {
        case TYPE_PTR:
        case TYPE_PTR_OWNED:
        case TYPE_STRUCT:
            return a.parameter._ptr == b.parameter._ptr;
        
        default:
            return true;
    }
}

bool operator!=(const Datatype& a, const Datatype& b) {
    return !(a==b);
}

//static Datatype createDatatypeStruct(StructDef* structDefPtr) {
//    Datatype datatype;
//    datatype.type = TYPE_STRUCT;
//    datatype.parameter._ptr = structDefPtr;
//}