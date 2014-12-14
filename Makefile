GCC=gcc
INCLUDES=-Iheaders
CFLAGS=-Os -g -std=c99 -Wall -flto -static
LDFLAGS=-Wl,--as-needed -Wl,-O1

HEADERS=$(wildcard headers/*.h)
SOURCES=$(wildcard src/*.c)
OBJECTS=$(SOURCES:.c=.o)

all: ${OBJECTS}

clean:
	rm -f ${OBJECTS}

src/%.o: src/%.c ${HEADERS}
	${GCC} -c ${INCLUDES} ${CFLAGS} $< -o $@

sha224sum: src/hash.o src/sha224sum.o
	${GCC} ${CFLAGS} ${LDFLAGS} $^ -o $@

test_hashtable: src/test_hashtable.o src/hashtable.o
	${GCC} ${CFLAGS} ${LDFLAGS} $^ -o $@
