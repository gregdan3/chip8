.PHONY: clean

IN=chip8.c
OUT=chip8
OUTD=chip8d

CFLAGS=-lm -O3
DEBUGFLAGS=-g -ggdb3 -O0
# later flag overrides

build: ${OUT}
debug: ${OUTD}
all: ${OUT} ${OUTD}

${OUT}: ${IN}
	gcc ${IN} -o ${OUT} ${CFLAGS}
${OUTD}: ${IN}
	gcc ${IN} -o ${OUTD} ${CFLAGS} ${DEBUGFLAGS}

clean:
	rm -f ${OUT} ${OUTD}
