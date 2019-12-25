.PHONY: all clean test

all: func

func: src/func.l src/func.y
	flex src/func.l
	bison -d src/func.y --verbose
	gcc func.tab.c lex.yy.c -o func

clean:
	rm func.tab.c lex.yy.c func.tab.h func func.output example.c example

test: func example.func
	./func < example.func > example.c
	gcc example.c -o example

