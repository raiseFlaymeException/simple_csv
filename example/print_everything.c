#include "simple_csv/scsv.h"

#define FILE_NAME "dogs.csv"
#define SEPARATOR ','

#define HEADERS_DEFAULT_CAP 1024

int main() {
    // create a file
    FILE *file = fopen(FILE_NAME, "r");
    if (file==NULL) {
        printf("Error opening file %s: %s", FILE_NAME, strerror(errno));
        return 1;
    }

    // allocate a string array for the headers
    ScsvSStringArr headers;
    ScsvSStringArr_alloc(&headers, HEADERS_DEFAULT_CAP);

    // parse an entire file and get an array of hashmap
    // headers can be NULL if you don't want it
    ScsvHashMapArr content = scsv_parse_entire_file(file, &headers, SEPARATOR);
    
    // print all headers
    printf("headers (%d): \n", headers.size);
    for (size_t i=0; i<headers.size; i++) {
        printf("header: %.*s\n", headers.data[i].size, headers.data[i].data);
    }

    // print all content
    for (size_t i=0; i<content.size; i++) {
        printf("*****************************\nidx %d:\n", i);
        for (size_t j=0; j<headers.size; j++) {
            // all function on data structures are name_*
            ScsvValue value;
            if (!ScsvHashMap_get_ptr(content.data+i, headers.data+j, &value)) {
                printf("invalid key get: %.*s\n", headers.data[j].size, headers.data[j].data);
                return 1;
            }

            printf("%.*s: ", headers.data[j].size, headers.data[j].data);
            switch (value.type) {
                case SCSV_STR:
                    printf("string: %.*s\n", value.str.size, value.str.data);
                    break;
                case SCSV_NUM:
                    printf("number: %lf\n", value.num);
                    break;
                case SCSV_NONE:
                    puts("none");
                    break;
                default:
                    puts("Invalid type");
                    return 1;
            }
        }
    }

    // free the string array 
    // (element are freed automaticly -> unlike normal easy_c_data_structure/array/carray_creator.h's array)
    ScsvSStringArr_free(&headers);
    // free the array of hashmap of string and CsvValue
    // (element are freed automaticly -> unlike normal easy_c_data_structure/array/carray_creator.h's array and
    //      easy_c_data_structure/hashmap/chashmap_creator.h's hashmap)
    ScsvHashMapArr_free(&content);

    fclose(file);
    return 0;
}
