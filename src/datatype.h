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

static datatype_t create_datatype(datatype_type_t type) {
    datatype_t datatype;
    datatype.type = type;
    return datatype;
}

static datatype_t create_datatype_label(const char* label) {
    size_t length = strlen(label);
    
    if (strcmp(label, "void") == 0)
        return create_datatype(TYPE_VOID);
    if (strcmp(label, "int") == 0)
        return create_datatype(TYPE_INT);
    if (strcmp(label, "char") == 0)
        return create_datatype(TYPE_S8);
    if (strcmp(label, "short") == 0)
        return create_datatype(TYPE_S16);
    if (strcmp(label, "long") == 0)
        return create_datatype(TYPE_S32);
    if (strcmp(label, "float") == 0)
        return create_datatype(TYPE_FLOAT32);
    if (strcmp(label, "double") == 0)
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

//static datatype_t create_datatype_tStruct(StructDef* structDefPtr) {
//    datatype_t datatype;
//    datatype.type = TYPE_STRUCT;
//    datatype._ptr = structDefPtr;
//}

#endif // DATATYPE_H