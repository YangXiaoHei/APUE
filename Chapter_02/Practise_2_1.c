// a.h
typedef unsigned long   __uint64_t;
typedef long            __int64_t;
typedef unsigned int    __uint32_t;
typedef int             __int32_t;
typedef unsigned short  __uint16_t;
typedef short           __int16_t;
typedef unsigned char   __uint8_t;
typedef char            __int8_t;

// b.h
#ifndef __UINT64_TYPE__
typedef __uint64_t uint64_t;
#define __UINT64_TYPE__
#endif

#ifndef __INT64_TYPE__
typedef __int64_t int64_t;
#define __INT64_TYPE__
#endif

#ifndef __UINT32_TYPE__
typedef __uint32_t uint32_t;
#define __UINT32_TYPE__
#endif

#ifndef __INT32_TYPE__
typedef __int32_t int32_t;
#define __INT32_TYPE__
#endif

#ifndef __UINT16_TYPE__
typedef __uint16_t uint16_t;
#define __UINT16_TYPE__
#endif

#ifndef __INT16_TYPE__
typedef __int16_t int16_t;
#define __INT16_TYPE__
#endif

#ifndef __UINT8_TYPE__
typedef __uint8_t uint8_t;
#define __UINT8_TYPE__
#endif

#ifndef __INT8_TYPE__
typedef __int8_t int8_t;
#define __INT8_TYPE__
#endif

int main() {
    
    
    return 0;
}
