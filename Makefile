.PHONY: all clean test

SRC_DIR=src/
SRC_FLEX=${SRC_DIR}func.l
SRC_BISON=${SRC_DIR}func.y

TMP_DIR=tmp/
TMP_FLEX=${TMP_DIR}lex.yy.c
TMP_BISON=${TMP_DIR}func.tab.c

BIN_DIR=bin/
BIN_NEW=${BIN_DIR}func
BIN_OLD=${BIN_DIR}func.old
BIN_OLDER=${BIN_DIR}func.older

all: ${BIN_NEW}

${BIN_NEW}: Makefile $(wildcard ${SRC_DIR}*)
ifeq ("$(wildcard ${BIN_OLD}) $(wildcard ${BIN_OLDER})", "${BIN_OLD} ${BIN_OLDER}")
	rm -rfv ${TMP_DIR}
	mkdir -p ${TMP_DIR}
	./${BIN_OLD} ${TMP_DIR}main.c ${SRC_DIR}main.func
	# || ./${BIN_OLDER} ${TMP_DIR}main.c ${SRC_DIR}main.func
endif
	flex --nodefault -o ${TMP_FLEX} ${SRC_FLEX}
	bison -d -o ${TMP_BISON} --verbose ${SRC_BISON}
	cp ${SRC_DIR}*.c ${TMP_DIR}
	cp ${SRC_DIR}*.h ${TMP_DIR}
	gcc -Wall -Wextra -D_GNU_SOURCE -o ${BIN_NEW} ${TMP_DIR}*.c
ifeq ("$(wildcard ${BIN_OLD})", "${BIN_OLD}")
	cp ${BIN_OLD} ${BIN_OLDER}
else
	cp ${BIN_NEW} ${BIN_OLDER}
endif
	cp ${BIN_NEW} ${BIN_OLD}

clean:
	rm -rfv ${TMP_DIR} ${BIN_NEW} example example.c

test: ${BIN_NEW} example.func
	./${BIN_NEW} example.c example.func
	gcc -Wall -Wextra example.c -o example
