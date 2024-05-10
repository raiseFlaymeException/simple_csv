#ifndef SCSV_H
#define SCSV_H

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "easy_c_data_structure/simple_string/sstring.h"
#include "easy_c_data_structure/hashmap/chashmap_creator.h"
#include "easy_c_data_structure/array/carray_creator.h"

#ifndef SCSV_STRING_DEFAULT_CAP
#define SCSV_STRING_DEFAULT_CAP 1024
#endif // SCSV_STRING_DEFAULT_CAP

#ifndef SCSV_SMALL_STRING_DEFAULT_CAP
#define SCSV_SMALL_STRING_DEFAULT_CAP 32
#endif // SCSV_SMALL_STRING_DEFAULT_CAP

#ifndef SCSV_ARRAY_DEFAULT_CAP 
#define SCSV_ARRAY_DEFAULT_CAP 1024
#endif // SCSV_ARRAY_DEFAULT_CAP 

#ifndef SCSV_HASHMAP_DEFAULT_CAP 
// close to 1024 and prime
#define SCSV_HASHMAP_DEFAULT_CAP 1021
#endif // SCSV_HASHMAP_DEFAULT_CAP 

typedef enum {
    SCSV_NONE,
    SCSV_NUM,
    SCSV_STR
} ScsvType;

typedef struct {
    ScsvType type;
    union {
        double num;
        SString str;
    };
} ScsvValue;

CHASHMAP_CREATE_DECLARATION(ScsvHashMap, SString, ScsvValue)
CARRAY_CREATE_DECLARATION(ScsvHashMapArr, ScsvHashMap)
CARRAY_CREATE_DECLARATION(ScsvSStringArr, SString)

ScsvHashMapArr scsv_parse_entire_file(FILE *file, ScsvSStringArr *headers, char separator);

// NOLINTBEGIN
void impl_ScsvSStringArr_free_rec(ScsvSStringArr *array);
void impl_ScsvHashMapArr_free_rec(ScsvHashMapArr *array);

#define ScsvSStringArr_free impl_ScsvSStringArr_free_rec
#define ScsvHashMapArr_free impl_ScsvHashMapArr_free_rec
// NOLINTEND

#endif // SCSV_H
