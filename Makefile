CC = gcc
CXX = g++
DEBUG = gdb

STANDARD = -std=c11

OPTI = -O3

WARNINGS = -Wall -Wextra -Wpedantic -Werror

HEADERDIR = src/include
LIBDIR = src/lib

LIBC_CPP = -static-libstdc++ -static-libgcc
HIDEWINDOW = -mwindows

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

HEADERS = $(call rwildcard,src/include,*.h)
EXTERNAL_HEADERS = $(call rwildcard,src/include/external,*.h)
INTERNAL_HEADERS = $(filter-out $(EXTERNAL_HEADERS),$(HEADERS))
SOURCES = $(call rwildcard,src/c,*.c)
COMPILED = $(subst .c,.o,$(SOURCES)) 

QUIET = > nul 2>&1

LIB =

all: bin/dog_breed_info.exe bin/print_everything.exe

bin/dog_breed_info.exe: example/dog_breed_info.o $(COMPILED)
	$(CC) example/dog_breed_info.o $(COMPILED) -o bin/dog_breed_info.exe $(LIB) -I $(HEADERDIR) -L $(LIBDIR) $(WARNING) $(STANDARD)

.PHONY: dog_breed_info_debug
dog_breed_info_debug: example/dog_breed_info.c $(SOURCES)
	$(CC) -ggdb3 example/dog_breed_info.c $(SOURCES) -o bin/dog_breed_info_debug.exe $(LIB) -I $(HEADERDIR) -L $(LIBDIR) $(WARNING) $(STANDARD)

.PHONY: run_dog_breed_info
run_dog_breed_info: bin/dog_breed_info.exe
	./bin/dog_breed_info.exe Beagle

example/dog_breed_info.o: example/dog_breed_info.c
	$(CC) -c example/dog_breed_info.c -o example/dog_breed_info.o -I $(HEADERDIR) $(WARNING) $(STANDARD)

bin/print_everything.exe: example/print_everything.o $(COMPILED)
	$(CC) example/print_everything.o $(COMPILED) -o bin/print_everything.exe $(LIB) -I $(HEADERDIR) -L $(LIBDIR) $(WARNING) $(STANDARD)

.PHONY: print_everything_debug
print_everything_debug: example/print_everything.c $(SOURCES)
	$(CC) -ggdb3 example/print_everything.c $(SOURCES) -o bin/print_everything_debug.exe $(LIB) -I $(HEADERDIR) -L $(LIBDIR) $(WARNING) $(STANDARD)

.PHONY: run_print_everything
run_print_everything: bin/print_everything.exe
	./bin/print_everything.exe

example/print_everything.o: example/print_everything.c
	$(CC) -c example/print_everything.c -o example/print_everything.o -I $(HEADERDIR) $(WARNING) $(STANDARD)

src/c/%.o: src/c/%.c
	$(CC) -c $< -o $@ -I $(HEADERDIR) $(WARNINGS) $(STANDARD)

.PHONY: clean
.SILENT: clean
clean:
	-del example\dog_breed_info.o $(QUIET)
	-del example\print_everything.o $(QUIET)
	-del /S src\c\*.o $(QUIET)

.PHONY: cleanmore
.SILENT: cleanmore
cleanmore: clean
	-del bin\*.exe $(QUIET)
