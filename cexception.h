#ifndef __cexception_h__
#define __cexception_h__

/* $Id: cexception.h,v 1.3 2023/01/15 03:53:23 swp Exp $
 *
 * EXAMPLE:
 *
 * void *xmalloc(int n) begin
 *     // throw(100);
 *     void *p = malloc(n);
 *     if (!p)
 *         throw(1);
 *     return p;
 * end
 * 
 * void cleanup_foo_p(void *volatile *p) {
 *     printf("%s(): %p\n", __func__, *p);
 *     if (*p)
 *         free(*p);
 * }
 * 
 * void foo() begin
 *     void *volatile p __attribute__((cleanup(cleanup_foo_p))) = NULL;
 *     try {
 *         p = xmalloc(1000);
 *         memset(p, 0, 1000);
 *     } catch(x) {
 *         printf("catch %d, rethrow.\n", x);
 *         throw(x);
 *     }
 * end
 * 
 * int main() begin
 *     try {
 *         foo();
 *         throw(2);
 *     } catch(x)
 *         printf("catch %d.\n", x);
 *     return 0;
 * end
 */

#include <stdlib.h>
#include <setjmp.h>

extern struct ef {
    struct ef *lnk;
    int val;
    unsigned init :1;
    unsigned exit :1;
    unsigned exception :1;
    jmp_buf env;
} *ef_top;

#define begin \
{ \
    __label__ _L_exception; \
    int ef_val; \
    {

static inline void __attribute__((always_inline)) cleanup_ef(struct ef *ef __attribute__((unused))) {
    ef_top = ef_top->lnk;
}
#define try     for (struct ef ef_ __attribute__((__cleanup__(cleanup_ef))) = \
                        { .lnk = ef_top, .val = 0, .init = 0, .exit = 0, .exception = 0, } \
                    ; \
                        ({ \
                            if (!ef_.init) { \
                                ef_.init = 1; \
                                ef_top = &ef_; \
                                ef_val = ef_.val = setjmp(ef_.env); \
                            } \
                            !ef_.exit; \
                        }) \
                    ; \
                    ef_.exit++ \
                ) \
                    if (!ef_.exception) { \
                        __label__ _L_exception; \
                        {

static inline void __attribute__((always_inline)) cleanup_catch(int *x __attribute__((unused))) {
    ef_top->exception = 0;
}
#define catch(x)            break; \
                        } \
                    _L_exception: \
                        ef_.exit = -1; \
                    } else \
                        for (int x __attribute__((__cleanup__(cleanup_catch))) = ef_.val; \
                            ef_.exception; ef_.exception = 0)

#define throw(x)    { ef_top->exception = 1; ef_top->val = ef_val = (x); goto _L_exception; }

#define end \
    } \
    if (0) { \
_L_exception: \
        if (ef_top) { \
            ef_top->exception = 1; \
            ef_top->val = ef_val; \
            longjmp(ef_top->env, ef_val); \
        } \
    } \
}

#endif

// vi: ts=4:sts=4:sw=4:et
