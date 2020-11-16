#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef __CORE_CRYPTO_H__
#define __CORE_CRYPTO_H__

#define ENABLED 1
#define SHA384_SUPPORT 1
#define TRUE true

#define ERROR_OUT_OF_MEMORY ENOMEM
#define NO_ERROR 0

#define osMemcpy memcpy

#define SWAPINT64(x) ( \
   (((uint64_t)(x) & 0x00000000000000FFULL) << 56) | \
   (((uint64_t)(x) & 0x000000000000FF00ULL) << 40) | \
   (((uint64_t)(x) & 0x0000000000FF0000ULL) << 24) | \
   (((uint64_t)(x) & 0x00000000FF000000ULL) << 8) | \
   (((uint64_t)(x) & 0x000000FF00000000ULL) >> 8) | \
   (((uint64_t)(x) & 0x0000FF0000000000ULL) >> 24) | \
   (((uint64_t)(x) & 0x00FF000000000000ULL) >> 40) | \
   (((uint64_t)(x) & 0xFF00000000000000ULL) >> 56))

#define betoh64(value) SWAPINT64((uint64_t) (value))
#define htobe64(value) SWAPINT64((uint64_t) (value))


#define ROR64(a, n) (((a) >> (n)) | ((a) << (64 - (n))))
#define SHR64(a, n) ((a) >> (n))

#define MIN(a,b) (((a) < (b)) ? (a) : (b))

extern unsigned char aucSha512Buffer[];

#define cryptoAllocMem(a) ((void*)(aucSha512Buffer))
#define cryptoFreeMem(a) ((void)(a))


typedef _Bool bool_t;
typedef char char_t;
typedef int error_t;
typedef unsigned int uint_t;


#endif  /* __CORE_CRYPTO_H__ */
