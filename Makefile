GCC=gcc
INCLUDES=-Iheaders
CFLAGS=-Os -Wall -flto -static
LDFLAGS=-Wl,--as-needed -Wl,-O1

HEADERS=$(wildcard headers/*.h)
SOURCES=$(wildcard src/*.c)
OBJECTS=$(SOURCES:.c=.o)

all: ${OBJECTS}

src/%.o: src/%.c ${HEADERS}
	${GCC} -c ${INCLUDES} ${CFLAGS} $< -o $@

sha224sum: src/hash.o src/sha224sum.o
	${GCC} ${CFLAGS} ${LDFLAGS} $? -o $@


