ARCH := $(shell uname -m)

CFLAGS = -ggdb3 -O2 -Wall -Iarch/${ARCH}

LIBUCONTEXT_C_SRC = \
	arch/${ARCH}/makecontext.c

LIBUCONTEXT_S_SRC = \
	arch/${ARCH}/getcontext.S \
	arch/${ARCH}/setcontext.S \
	arch/${ARCH}/swapcontext.S \
	arch/${ARCH}/startcontext.S

LIBUCONTEXT_OBJ = ${LIBUCONTEXT_C_SRC:.c=.o} ${LIBUCONTEXT_S_SRC:.S=.o}
LIBUCONTEXT_SOVERSION = 0
LIBUCONTEXT_NAME = libucontext.so
LIBUCONTEXT_SONAME = libucontext.so.${LIBUCONTEXT_SOVERSION}
LIBUCONTEXT_PATH = /lib/${LIBUCONTEXT_SONAME}

all: ${LIBUCONTEXT_SONAME}

${LIBUCONTEXT_NAME}: ${LIBUCONTEXT_OBJ}
	$(CC) -o ${LIBUCONTEXT_NAME} -Wl,-soname,${LIBUCONTEXT_SONAME} \
		-shared ${LIBUCONTEXT_OBJ}

${LIBUCONTEXT_SONAME}: ${LIBUCONTEXT_NAME}
	ln -sf ${LIBUCONTEXT_NAME} ${LIBUCONTEXT_SONAME}

.c.o:
	$(CC) -std=c99 -D_BSD_SOURCE -fPIC -DPIC ${CFLAGS} ${CPPFLAGS} -c -o $@ $<

.S.o:
	$(CC) -fPIC -DPIC ${CFLAGS} ${CPPFLAGS} -c -o $@ $<

clean:
	rm -f ${LIBUCONTEXT_NAME} ${LIBUCONTEXT_SONAME} ${LIBUCONTEXT_OBJ}

install: all
	install -D -m755 ${LIBUCONTEXT_NAME} ${DESTDIR}/${LIBUCONTEXT_PATH}
	ln -sf ${LIBUCONTEXT_SONAME} ${DESTDIR}/lib/${LIBUCONTEXT_NAME}

check: test_libucontext ${LIBUCONTEXT_SONAME}
	env LD_LIBRARY_PATH=. ./test_libucontext

test_libucontext: test_libucontext.c ${LIBUCONTEXT_NAME}
	$(CC) -std=c99 -D_BSD_SOURCE ${CFLAGS} ${CPPFLAGS} $@.c -o $@ ${LIBUCONTEXT_NAME}

.PHONY: check
