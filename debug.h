#ifndef COAP_DEBUG_H
#define COAP_DEBUG_H 1
#include <stdio.h>

#ifdef DEBUG
#define PDEBUG(...) \
 do { \
        printf(__VA_ARGS__); \
    } while (0)
#else
#define PDEBUG(...)
#endif

#endif /* COAP_DEBUG_H */