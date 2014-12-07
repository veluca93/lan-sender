GCC=gcc
INCLUDES=-Iheaders
CFLAGS=-Os -Wall -flto -static
LDFLAGS=-Wl,--as-needed -Wl,-O1

HEADERS=$(wildcard headers/*.h)

all: hashtable

src/%.o: src/%.c ${HEADERS}
	${GCC} -c ${INCLUDES} ${CFLAGS} $< -o $@

hashtable: src/hashtable.o
	${GCC} ${CFLAGS} ${LDFLAGS} $? -o $@
