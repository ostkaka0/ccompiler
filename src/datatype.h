#ifndef DATATYPE_H
#define DATATYPE_H

#include <string.h>
#include <stdbool.h>

#include "core/str.h"

//struct DataType;
struct Datatype_tag;

typedef enum {
    TYPE_UNEVALUATED = 0,
    TYPE_VOID,
    
    // Types without parameters
    TYPE_RAWPTR,
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
} DatatypeTag;

// typedef union {
//     struct Datatype* _ptr;
//     //StructDef* _struct;
// } datatype_parameter_t;

typedef struct DatatypeTag {
    //datatype_parameter_t parameter;
	union {
        struct DatatypeTag* _ptr;
		//StructDef* _struct;
	};
    DatatypeTag tag;
} Datatype;

static bool _datatype_implicit_cast_allowed(const Datatype a, const Datatype b);

static Datatype create_datatype(DatatypeTag tag) {
    Datatype datatype;
    datatype.tag = tag;
    return datatype;
}

static Datatype create_datatype_label(Str label) {
    if (str_is(label, "void"))
        return create_datatype(TYPE_VOID);
    if (str_is(label, "rawptr"))
        return create_datatype(TYPE_RAWPTR);
    if (str_is(label, "int"))
        return create_datatype(TYPE_INT);
    if (str_is(label, "s8"))
        return create_datatype(TYPE_S8);
    if (str_is(label, "s16"))
        return create_datatype(TYPE_S16);
    if (str_is(label, "s32"))
        return create_datatype(TYPE_S32);
    if (str_is(label, "float32"))
        return create_datatype(TYPE_FLOAT32);
    if (str_is(label, "float64"))
        return create_datatype(TYPE_FLOAT64);
    
    return create_datatype(TYPE_UNEVALUATED);
}

static Datatype create_datatype_ptr(Datatype* datatypePtr) {
    Datatype datatype;
    datatype.tag = TYPE_PTR;
    datatype._ptr = datatypePtr;
}

static Datatype create_datatype_ptr_owned(Datatype* datatypePtr) {
    Datatype datatype;
    datatype.tag = TYPE_PTR_OWNED;
    datatype._ptr = datatypePtr;
}

static bool datatype_equals(const Datatype a, const Datatype b) {
    if (a.tag != b.tag)
        return false;
    
    switch (a.tag) {
        case TYPE_PTR:
        case TYPE_PTR_OWNED:
        case TYPE_STRUCT:
            return a._ptr == b._ptr;
        
        default:
            return true;
    }
}



static bool datatype_implicit_cast_equals(const Datatype a, const Datatype b) {
    return (datatype_equals(a, b) ||
        _datatype_implicit_cast_allowed(a, b) ||
        _datatype_implicit_cast_allowed(b, a));
}

static bool _datatype_implicit_cast_allowed(const Datatype a, const Datatype b) {
    switch(b.tag) {
        case TYPE_INT:
            return (a.tag == TYPE_S8 || a.tag == TYPE_S16 || a.tag == TYPE_S32 || a.tag == TYPE_S64);
        case TYPE_UINT:
            return (a.tag == TYPE_U8 || a.tag == TYPE_U16 || a.tag == TYPE_U32 || a.tag == TYPE_U64);
        case TYPE_RAWPTR:
            return (a.tag == TYPE_PTR || a.tag == TYPE_PTR_OWNED);
        case TYPE_PTR_OWNED:
           return (a.tag == TYPE_PTR && datatype_implicit_cast_equals(*a._ptr, *b._ptr));
    }
    return false;
}

static Str datatype_to_str(const Datatype datatype) {
    switch(datatype.tag) {
    case TYPE_UNEVALUATED: return str("UNEVALUATED");
    case TYPE_VOID: return str("void");
    case TYPE_RAWPTR: return str("rawptr");

    // Types without parameters
    case TYPE_SIZE_T: return str("size_t");
    case TYPE_UINT: return str("uint");
    case TYPE_INT: return str("int");
    case TYPE_U8: return str("u8");
    case TYPE_S8: return str("s8");
    case TYPE_U16: return str("u16");
    case TYPE_S16: return str("s16");
    case TYPE_U32: return str("u32");
    case TYPE_S32: return str("s32");
    case TYPE_U64: return str("u64");
    case TYPE_S64: return str("s64");
    case TYPE_FLOAT32: return str("float32");
    case TYPE_FLOAT64: return str("float64");

    // Types with parameters
    case TYPE_PTR: // Pointer
        break;
    case TYPE_PTR_OWNED:
        break;
    case TYPE_STRUCT:
        break;
    default:
        break;
    }
    return str("UNDEFINED");
}

static Str datatype_to_str_as_c(const Datatype datatype) {
    switch(datatype.tag) {
    case TYPE_UNEVALUATED: return str("UNEVALUATED");
    case TYPE_VOID: return str("void");
    case TYPE_RAWPTR: return str("void*");

    // Types without parameters
    case TYPE_SIZE_T: return str("size_t");
    case TYPE_UINT: return str("uintptr_t");
    case TYPE_INT: return str("intptr_t");
    case TYPE_U8: return str("uint8_t");
    case TYPE_S8: return str("int8_t");
    case TYPE_U16: return str("uint16_t");
    case TYPE_S16: return str("int16_t");
    case TYPE_U32: return str("uint32_t");
    case TYPE_S32: return str("int32_t");
    case TYPE_U64: return str("uint64_t");
    case TYPE_S64: return str("int64_t");
    case TYPE_FLOAT32: return str("float");
    case TYPE_FLOAT64: return str("double");

    // Types with parameters
    case TYPE_PTR: // Pointer
        break;
    case TYPE_PTR_OWNED:
        break;
    case TYPE_STRUCT:
        break;
    default:
        break;
    }
    return str("UNDEFINED");
}

//static Datatype create_DatatypeStruct(StructDef* structDefPtr) {
//    Datatype datatype;
//    datatype.type = TYPE_STRUCT;
//    datatype._ptr = structDefPtr;
//}

#endif // DATATYPE_H
