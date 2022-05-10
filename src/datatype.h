#ifndef DATATYPE_H
#define DATATYPE_H

#include <string.h>
#include <stdbool.h>

//struct DataType;
struct datatype_t_tag;

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
} datatype_type_t;

// typedef union {
//     struct datatype_t* _ptr;
//     //StructDef* _struct;
// } datatype_parameter_t;

typedef struct datatype_t_tag {
    //datatype_parameter_t parameter;
	union {
		struct datatype_t_tag* _ptr;
		//StructDef* _struct;
	};
    datatype_type_t type;
} datatype_t;

static bool _datatype_implicit_cast_allowed(const datatype_t a, const datatype_t b);

static datatype_t create_datatype(datatype_type_t type) {
    datatype_t datatype;
    datatype.type = type;
    return datatype;
}

static datatype_t create_datatype_label(const char* label) {
    size_t length = strlen(label);
    
    if (strcmp(label, "void") == 0)
        return create_datatype(TYPE_VOID);
    if (strcmp(label, "rawptr") == 0)
        return create_datatype(TYPE_RAWPTR);
    if (strcmp(label, "int") == 0)
        return create_datatype(TYPE_INT);
    if (strcmp(label, "s8") == 0)
        return create_datatype(TYPE_S8);
    if (strcmp(label, "s16") == 0)
        return create_datatype(TYPE_S16);
    if (strcmp(label, "s32") == 0)
        return create_datatype(TYPE_S32);
    if (strcmp(label, "float32") == 0)
        return create_datatype(TYPE_FLOAT32);
    if (strcmp(label, "float64") == 0)
        return create_datatype(TYPE_FLOAT64);
    
    return create_datatype(TYPE_UNEVALUATED);
}

static datatype_t create_datatype_ptr(datatype_t* datatypePtr) {
    datatype_t datatype;
    datatype.type = TYPE_PTR;
    datatype._ptr = datatypePtr;
}

static datatype_t create_datatype_ptr_owned(datatype_t* datatypePtr) {
    datatype_t datatype;
    datatype.type = TYPE_PTR_OWNED;
    datatype._ptr = datatypePtr;
}

static bool datatype_cmp(const datatype_t a, const datatype_t b) {
    if (a.type != b.type)
        return false;
    
    switch (a.type) {
        case TYPE_PTR:
        case TYPE_PTR_OWNED:
        case TYPE_STRUCT:
            return a._ptr == b._ptr;
        
        default:
            return true;
    }
}



static bool datatype_implicit_cast_cmp(const datatype_t a, const datatype_t b) {
    return (datatype_cmp(a, b) ||
        _datatype_implicit_cast_allowed(a, b) ||
        _datatype_implicit_cast_allowed(b, a));
}

static bool _datatype_implicit_cast_allowed(const datatype_t a, const datatype_t b) {
    switch(b.type) {
        case TYPE_INT:
            return (a.type == TYPE_S8 || a.type == TYPE_S16 || a.type == TYPE_S32 || a.type == TYPE_S64);
        case TYPE_UINT:
            return (a.type == TYPE_U8 || a.type == TYPE_U16 || a.type == TYPE_U32 || a.type == TYPE_U64);
        case TYPE_RAWPTR:
            return (a.type == TYPE_PTR || a.type == TYPE_PTR_OWNED);
        case TYPE_PTR_OWNED:
           return (a.type == TYPE_PTR && datatype_implicit_cast_cmp(*a._ptr, *b._ptr));
    }
    return false;
}

static char* datatype_to_string(const datatype_t datatype) {
    switch(datatype.type) {
    case TYPE_UNEVALUATED: return "UNEVALUATED";
    case TYPE_VOID: return "void";
    case TYPE_RAWPTR: return "rawptr";

    // Types without parameters
    case TYPE_SIZE_T: return "size_t";
    case TYPE_UINT: return "uint";
    case TYPE_INT: return "int";
    case TYPE_U8: return "u8";
    case TYPE_S8: return "s8";
    case TYPE_U16: return "u16";
    case TYPE_S16: return "s16";
    case TYPE_U32: return "u32";
    case TYPE_S32: return "s32";
    case TYPE_U64: return "u64";
    case TYPE_S64: return "s64";
    case TYPE_FLOAT32: return "float32";
    case TYPE_FLOAT64: return "float64";

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
    return "UNDEFINED";
}

//static datatype_t create_datatype_tStruct(StructDef* structDefPtr) {
//    datatype_t datatype;
//    datatype.type = TYPE_STRUCT;
//    datatype._ptr = structDefPtr;
//}

#endif // DATATYPE_H
