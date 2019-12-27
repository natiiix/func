.PHONY: all clean test

all: func

func: $(wildcard src/*)
	flex src/func.l
	bison -d src/func.y --verbose
	gcc -Wall -Wextra -o func func.tab.c lex.yy.c $(wildcard src/*.c)

clean:
	rm func.tab.c lex.yy.c func.tab.h func func.output example.c example

test: func example.func
	./func < example.func > example.c
	gcc example.c -o example

