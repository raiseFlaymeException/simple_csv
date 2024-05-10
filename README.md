# simple CSV

A c library to parse csv file.

- Support double quotes / non quoted string.
- Support one character separator (not including '"' and '\n').
- Support new line inside the content (not inside the header).
- Support two consecutive double quotes signifying a single double quotes.


## Table of Contents

- [Install](#install)
- [Usage](#usage)
- [Example](#example)
- [Maintainers](#maintainers)
- [Contributing](#contributing)
- [License](#license)

## Install

compile and run the test program:
```cmd
make run

```

- the library includes are in the folder [src/include/simple_csv](src/include/simple_csv)
- the library source code are in the folder [src/c/simple_csv](src/c/simple_csv)
- the library depend of [easy_c_data_stucure](https://github.com/raiseFlaymeException/easy-c-data-structure) located in [src/include/easy_c_data_structure](src/include/easy_c_data_structure)

## Usage

I try to comment stuff in the [example](#example) and in the library to make it easier

read a csv file:
```c
// allocate a string array for the headers
ScsvSStringArr headers;
ScsvSStringArr_alloc(&headers, HEADERS_DEFAULT_CAP);

// parse an entire file and get an array of hashmap
// headers can be NULL if you don't want it
ScsvHashMapArr content = scsv_parse_entire_file(file, &headers, SEPARATOR);
```

do stuff with the content / headers:
see [easy_c_data_stucure](https://github.com/raiseFlaymeException/easy-c-data-structure)

free the content and headers:
```c
// free the string array 
// (element are freed automaticly -> unlike normal easy_c_data_structure/array/carray_creator.h's array)
ScsvSStringArr_free(&headers);
// free the array of hashmap of string and CsvValue
// (element are freed automaticly -> unlike normal easy_c_data_structure/array/carray_creator.h's array and
//      easy_c_data_structure/hashmap/chashmap_creator.h's hashmap)
ScsvHashMapArr_free(&content);
```

## Example

A better example in the file [example.c](example.c)

## Maintainers

[@raiseFlaymeException](https://github.com/raiseFlaymeException).

## Contributing

Feel free to contibute [Open an issue](https://github.com/raiseFlaymeException/simple_csv/issues/new) or submit PRs.

## License

[ZLIB](LICENSE) © raiseFlaymeException
