.PHONY: all clean test

all: func

func: Makefile $(wildcard src/*)
	rm -rfv tmp/*
	mkdir -p tmp/
	flex --nodefault -o tmp/lex.yy.c src/func.l
	bison -d -o tmp/func.tab.c --verbose src/func.y
	cp src/*.c tmp/
	cp src/*.h tmp/
	gcc -Wall -Wextra -o func tmp/*.c

clean:
	rm -rfv tmp/ func example example.c

test: func example.func
	./func example.c example.func
	gcc -Wall -Wextra example.c -o example
