
FLAGS=
CFLAGS=
LDFLAGS=
LDLIBS=

CFLAGS += -std=c11 -Wall -Wextra -pedantic -D_DEFAULT_SOURCE
CFLAGS += -fstack-protector-all
FLAGS  += -fsanitize=address,undefined,bounds
#FLAGS  += -fsanitize=undefined,bounds
CFLAGS += -g -Og
CFLAGS += -Iinclude/

CFLAGS += $(FLAGS)
LDFLAGS += $(FLAGS)

# LDLIBS += -lasan
LDLIBS += -ldpa-utils -lgmp

OBJECTS += build/main.o
OBJECTS += build/tokenizer.o
OBJECTS += build/parser.o
OBJECTS += build/ast.o
OBJECTS += build/gen/ast.o

all: main

include/ast.h: include/ast.h.in gen_ast.py
	./gen_ast.py

src/gen/ast.c: include/ast.h

build/%.o: src/%.c $(shell find include/ -iname "*.h")
	mkdir -p $(dir $@)
	$(CC) -c -o $@ $(CFLAGS) $<

main: $(OBJECTS)
	$(CC) -o $@ $(LDFLAGS) $^ $(LDLIBS)

clean:
	rm -f main $(OBJECTS)
