#include "simple_csv/scsv.h"

#define FILE_NAME "dogs.csv"
#define SEPARATOR ','

#define HEADERS_DEFAULT_CAP 1024

int main(int argc, char *argv[]) {
    if (argc!=2) {
        assert(argc>=1);
        printf("Usage: %s <dog breed>\n", argv[0]);
        return 1;
    }

    const char *dog_breed = argv[1];

    FILE *file = fopen(FILE_NAME, "r");
    if (file==NULL) {
        printf("Error opening file %s: %s", FILE_NAME, strerror(errno));
        return 1;
    }

    ScsvSStringArr headers;
    ScsvSStringArr_alloc(&headers, HEADERS_DEFAULT_CAP);

    ScsvHashMapArr content = scsv_parse_entire_file(file, &headers, SEPARATOR);

    size_t idx_result = -1;
    for (size_t i=0; i<content.size; i++) {
        ScsvValue val;
        if (!ScsvHashMap_get_ptr(content.data+i, headers.data, &val)) { // we know that headers.data[0] is the string Dog Breed
            printf("Something went wrong, unable to find Dog Breed");
            return 1;
        }
        if (val.type==SCSV_STR && val.str.size==strlen(dog_breed) && memcmp(val.str.data, dog_breed, val.str.size)==0) {
            idx_result = i;
            break;
        }
    }
    if (idx_result==(size_t)-1) {
        printf("Invalid dog breed: %s\n", dog_breed);
        return 1;
    }

    for (size_t i=0; i<headers.size; i++) {
        ScsvValue val;
        if (!ScsvHashMap_get_ptr(content.data+idx_result, headers.data+i, &val)) {
            printf("Something went wrong, unable to find %.*s\n", headers.data[i].size, headers.data[i].data);
            return 1;
        }
        switch (val.type) {
            case SCSV_STR:
                printf("%.*s: %.*s\n", headers.data[i].size, headers.data[i].data, val.str.size, val.str.data);
                break;

            case SCSV_NUM:
                printf("%.*s: %d\n", headers.data[i].size, headers.data[i].data, (int)val.num); // we convert to int so it's easier to see
                break;

            default:
                break;
        }
    }

    ScsvSStringArr_free(&headers);
    ScsvHashMapArr_free(&content);

    fclose(file);
    return 0;
}
