LIB= cexception
SHLIB_MAJOR= 1
SHLIB_MINOR= 0
INCS+= cexception.h
SRCS+= cexception.c
CPUTYPE= native
#CFLAGS+= -DNDEBUG
CFLAGS+= -I${.CURDIR} -I/usr/local/include
CSTD= gnu17
#CC= gcc10
#LDFLAGS+= -Wl,--rpath=/usr/local/lib/gcc10

LIBDIR= /usr/local/lib
INCLUDEDIR= /usr/local/include

.include <bsd.lib.mk>
