GCC=gcc
INCLUDES=-Iheaders
CFLAGS=-Os -std=c99 -Wall -flto -static
LDFLAGS=-Wl,--as-needed -Wl,-O1

HEADERS=$(wildcard headers/*.h)
SOURCES=$(wildcard src/*.c)
OBJECTS=$(SOURCES:.c=.o)

all: ${OBJECTS}

src/%.o: src/%.c ${HEADERS}
	${GCC} -c ${INCLUDES} ${CFLAGS} $< -o $@

sha224sum: src/hash.o src/sha224sum.o
	${GCC} ${CFLAGS} ${LDFLAGS} $? -o $@

test_hash: src/test_hashtable.o src/hashtable.o
	${GCC} ${CFLAGS} ${LDFLAGS} $? -o $@
