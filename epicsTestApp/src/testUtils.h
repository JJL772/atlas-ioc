
#pragma once

#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include <epicsTime.h>

#ifdef __ALTIVEC__
#include <altivec.h>
// Workaround for GCC #58241
#undef bool
#endif

#define STACK_STR_SIZE 512

/**
 * Simple string container that lives on the stack
 * Used to avoid having to pass in buffer pointers and sizes
 */
class StackStr {
public:
    StackStr() { data[0] = 0; }

    StackStr(const StackStr& rhs) {
        strncpy(data, rhs.data, max_size());
    }

    const char* get() const { return data; }
    char* get() { return data; }

    size_t max_size() const { return STACK_STR_SIZE; }
    size_t length() const { return strlen(data); }

    operator char*() {
        return data;
    }

    operator const char*() const {
        return data;
    }
private:
    char data[512];
};

/**
 * Returns current time in a nice format
 */
char* get_time(char* buf, size_t maxsize, epicsTimeStamp* ts = NULL);
StackStr get_time(epicsTimeStamp* ts = NULL);

/**
 * \brief One's complement sum between lhs and rhs
 */
static inline uint16_t ones_sum(uint16_t lhs, uint16_t rhs) {
    int a = lhs + rhs;
    return (a & 0xFFFF) + (a >> 16);
}

static inline uint16_t ip_cksum(const void* data, size_t len) {
	size_t rem = len % 2;
	len /= 2;
	uint16_t s = 0;
	for (size_t i = 0; i < len; ++i)
		s = ones_sum(s, *((const uint16_t*)(data) + i));
	if (rem) {
		union { uint16_t a; uint8_t b[2]; } tb = {0};
		tb.b[0] = *(((const uint8_t*)data) + len*2);
		return ones_sum(s, tb.a);
	}
	return ~s;
}

static inline void* aligned_alloc_p(size_t align, size_t size) {
    char* r = (char*)malloc(size + align);
    return r + ((uintptr_t)r) % align;
}

#define ALIGNED(x) __attribute__((aligned(x)))

#define ALIGN_VALUE(_val, _align) ((_val) + ((_val) % (_align)))

/* Dumb pack, not consistent with different endian platforms */
#define PACK_U32(a,b,c,d) ((a)<<24 | (b)<<16 | (c)<<8 | (d))

static inline bool memcheck_fast(const void* mem, uint8_t bv, size_t size) {
    #define MD(_a, _b) (size_t)(((char*)_a) - ((char*)_b))
    uint32_t value = PACK_U32(bv, bv, bv, bv);
#if __ALTIVEC__
    const __vector uint32_t compare = {value, value, value, value};
    /* Process one 128-bit bunch at a time */
    for (uint32_t* p = (uint32_t*)mem; MD(p, mem) < size; p += 4) {
        __builtin_prefetch(p + 4);
        if (!vec_all_eq(vec_ld(0, p), compare))
            return false;
    }
    return true;
#else
    for (uint32_t* p = (uint32_t*)mem; MD(p, mem) < size; p++)
        if (*p != value)
            return false;
    return true;
#endif
    #undef MD
}
