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

all: bin/example.exe

bin/example.exe: example.o $(COMPILED)
	$(CC) example.o $(COMPILED) -o bin/example.exe $(LIB) -I $(HEADERDIR) -L $(LIBDIR) $(WARNING) $(STANDARD)

.PHONY: example_debug
example_debug: example.c $(SOURCES)
	$(CC) -ggdb3 example.c $(SOURCES) -o bin/example_debug.exe $(LIB) -I $(HEADERDIR) -L $(LIBDIR) $(WARNING) $(STANDARD)

.PHONY: example_preprocess
example_preprocess: example.c $(COMPILED) $(SOURCES)
	$(CC) -E $(SOURCES) $(COMPILED) $(LIB) -I $(HEADERDIR) -L $(LIBDIR) $(WARNING) $(STANDARD) $(OPTI) > bin/example.ipp

.PHONY: debug
debug: example_debug
	$(DEBUG) bin/example_debug.exe

.PHONY: release_bin
release_bin: example.c $(SOURCES)
	$(CC) example.c $(SOURCES) -o bin/example_release.exe $(LIB) $(LIBC_CPP) -I $(HEADERDIR) -L $(LIBDIR) -D NDEBUG $(WARNING) $(STANDARD) $(OPTI)

.PHONY: run
run: bin/example.exe
	./bin/example.exe

.PHONY: rebuild
rebuild: clean bin/example.exe

.PHONY: rerun
rerun: clean run

.PHONY: clang-tidy
clang-tidy: $(INTERNAL_HEADERS) $(SOURCES)
	clang-tidy $(INTERNAL_HEADERS) $(SOURCES)

code-action: $(INTERNAL_HEADERS) $(SOURCES)
	clang-tidy $(INTERNAL_HEADERS) $(SOURCES) -fix -fix-errors

format: $(INTERNAL_HEADERS) $(SOURCES)
	clang-format $(INTERNAL_HEADERS) $(SOURCES) -i

example.o: example.c
	$(CC) -c example.c -o example.o -I $(HEADERDIR) $(WARNING) $(STANDARD)

src/c/%.o: src/c/%.c
	$(CC) -c $< -o $@ -I $(HEADERDIR) $(WARNINGS) $(STANDARD)

.PHONY: clean
.SILENT: clean
clean:
	-del example.o $(QUIET)
	-del /S src\c\*.o $(QUIET)

.PHONY: cleanmore
.SILENT: cleanmore
cleanmore: clean
	-del bin\*.exe $(QUIET)

	-del bin\example.ipp $(QUIET)
