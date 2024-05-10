#include "simple_csv/scsv.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#ifndef SSTRING_IMPL
#define SSTRING_IMPL
#endif // SSTRING_IMPL
#include "easy_c_data_structure/simple_string/sstring.h"
#include "easy_c_data_structure/hashmap/chashmap_creator.h"
#include "easy_c_data_structure/array/carray_creator.h"

// undef so we can call function normally
#undef ScsvSStringArr_free
#undef ScsvHashMapArr_free

#define DJB2_START 5381

size_t SString_hash(const SString *str) { // NOLINT
    // djb2
    unsigned long hash = DJB2_START;
    int chr;

    for (size_t i=0; i<str->size; i++) {
        chr = *str->data;
        hash = ((hash << 5) + hash) + chr; /* hash * 33 + c */ // NOLINT
    }

    return hash;
}

bool SString_equals(const SString *str1, const SString *str2) { // NOLINT
    return str1->size==str2->size && (memcmp(str1->data, str2->data, str1->size) == 0);
}

CHASHMAP_CREATE_DEFINITION(ScsvHashMap, SString, ScsvValue, SString_equals, SString_hash)
CARRAY_CREATE_DEFINITION(ScsvHashMapArr, ScsvHashMap)
CARRAY_CREATE_DEFINITION(ScsvSStringArr, SString)


size_t scsv_parse_until_stop(SStringAll *line, ScsvSStringArr *result, size_t idx_begin, size_t size_stop, char separator) {
    assert(separator!='"' && "separator can't be '\"'");
    assert(separator!='\n' && "separator can't be '\\n'");

    size_t idx_end=idx_begin;
    bool inside_quotes=false;
    for (size_t i=idx_begin;i<line->size && result->size < size_stop; i++) {
        char chr=line->data[i];
        if (chr==separator) {
            if (!inside_quotes) {
                SStringView view;
                SStringView_init_substr(&view, line, idx_begin, idx_end-idx_begin);

                SString value;
                SString_alloc(&value, SCSV_SMALL_STRING_DEFAULT_CAP);
                SString_append_str(&value, (SStringAll *)&view);
                ScsvSStringArr_append(result, value);

                idx_begin = idx_end+1;
            }
        } else if (chr=='"') {
            if (inside_quotes) {
                if (i+1<line->size && line->data[i+1]=='"') {
                    i++;
                    idx_end++;
                } else {
                    inside_quotes=false;

                    idx_end++;
                    SStringView view;
                    SStringView_init_substr(&view, line, idx_begin+1, idx_end-idx_begin-2);

                    SString value;
                    SString_alloc(&value, SCSV_SMALL_STRING_DEFAULT_CAP);
                    SString_append_str(&value, (SStringAll *)&view);

                    size_t idx_end_quotes=value.size;
                    for (size_t i=0; i<idx_end_quotes; i++) {
                        if (value.data[i]=='"') {
                            SString_pop(&value, i);
                            idx_end_quotes--;
                            i++;
                        }
                    }

                    ScsvSStringArr_append(result, value);

                    idx_begin = idx_end+1;
                    i++;
                }
            } else {
                inside_quotes=true;
            }
        } else if (chr=='\n') {
            if (!inside_quotes && result->size+1==size_stop) {
                SStringView view;
                SStringView_init_substr(&view, line, idx_begin, idx_end-idx_begin);

                SString value;
                SString_alloc(&value, SCSV_SMALL_STRING_DEFAULT_CAP);
                SString_append_str(&value, (SStringAll *)&view);

                size_t idx_end_quotes=value.size;
                for (size_t i=0; i<idx_end_quotes; i++) {
                    if (value.data[i]=='"') {
                        SString_pop(&value, i);
                        idx_end_quotes--;
                        i++;
                    }
                }

                ScsvSStringArr_append(result, value);
                return idx_end;
            }
        }
        idx_end++;
    }
    if (result->size < size_stop) {
        SStringView view;
        SStringView_init_substr(&view, line, idx_begin, line->size-idx_begin);

        SString value;
        SString_alloc(&value, SCSV_SMALL_STRING_DEFAULT_CAP);
        SString_append_str(&value, (SStringAll *)&view);
        ScsvSStringArr_append(result, value);
    }
    return idx_end;
}


ScsvHashMapArr scsv_parse_entire_file(FILE *file, ScsvSStringArr *headers, char separator) {
    ScsvSStringArr headers_null;
    bool headers_is_null = headers==NULL;
    if (headers_is_null) {
        ScsvSStringArr_alloc(&headers_null, SCSV_ARRAY_DEFAULT_CAP);
        headers = &headers_null;
    }

    // read the header from the file
    SString headers_str;
    SString_alloc(&headers_str, SCSV_STRING_DEFAULT_CAP);
    SString_getc_until(&headers_str, file, '\n');

    // parse headers
    // (size_t)-1 to read the entire line
    scsv_parse_until_stop((SStringAll *)&headers_str, headers, 0, (size_t)-1, separator);

    // read the content from the file
    SString content_str;
    SString_alloc(&content_str, SCSV_STRING_DEFAULT_CAP);
    SString_getc_until(&content_str, file, EOF);

    size_t number_lines=SStringAll_countc((SStringAll *)&content_str, '\n', 0);

    ScsvHashMapArr content;
    ScsvHashMapArr_alloc(&content, number_lines);

    size_t idx_char=0;
    size_t idx_line=0;
    while (true) {
        ScsvSStringArr scsv_string_arr;
        ScsvSStringArr_alloc(&scsv_string_arr, headers->size);

        idx_char=scsv_parse_until_stop((SStringAll *)&content_str, &scsv_string_arr, idx_char, headers->size, separator)+1;

        if (scsv_string_arr.size<=1) {
            // appen when there is no data left in the scsv file

            // free string inside of scsv string arr because not copied to the hashmap
            SString_free(scsv_string_arr.data);
            ScsvSStringArr_free(&scsv_string_arr);
            break;
        }

        assert(scsv_string_arr.size==headers->size);


        ScsvHashMapArr_extend(&content, 1);
        content.size++;

        ScsvHashMap_alloc(content.data+idx_line, headers->size);

        for (size_t i=0; i<scsv_string_arr.size; i++) {
            // assume it's a string
            ScsvValue value;
            value.type = SCSV_STR;
            value.str = scsv_string_arr.data[i];

            char *str_cpy;
            // check if is a number
            double num = strtod(value.str.data, &str_cpy);

            if (*value.str.data=='0') {
                // start with 0 is a string 
            } else if (value.str.size==0) {
                // check if null
                value.type = SCSV_NONE;
                SString_free(scsv_string_arr.data+i);
            } else if ((size_t)(str_cpy-value.str.data)==value.str.size) { // str_cpy always larget than value.str.data
                // dif of pointers == size read
                value.type = SCSV_NUM;
                SString_free(scsv_string_arr.data+i);
                value.num = num;
            }

            ScsvHashMap_insert(content.data+idx_line, headers->data[i], value);
        }

        // free sstring laters
        ScsvSStringArr_free(&scsv_string_arr);

        idx_line++;
    }


    // free string buffers
    SString_free(&headers_str);
    SString_free(&content_str);

    if (headers_is_null) {
        ScsvSStringArr_free(&headers_null);
    }

    return content;
}

void _ScsvSStringArr_free_rec(ScsvSStringArr *array) { // NOLINT
    for (size_t i=0; i<array->size; i++) {
        SString_free(array->data+i);
    }
    ScsvSStringArr_free(array);
}

void _ScsvHashMapArr_free_rec(ScsvHashMapArr *array) { // NOLINT
    for (size_t i=0; i<array->size; i++) {
        for (size_t j=0; j<array->data[i].size; j++) {
            if (array->data[i].data[j].value.type==SCSV_STR) {
                SString_free(&array->data[i].data[j].value.str);
            }
        }
        ScsvHashMap_free(array->data+i);
    }
    ScsvHashMapArr_free(array);
}
