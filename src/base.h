#ifndef __BASE__
#define __BASE__

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define VOLK_IMPLEMENTATION
#include <volk/volk.h>
#include <GLFW/glfw3.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

typedef float f32;
typedef double f64;

typedef uint32_t b32;
typedef size_t usize;

#define true  1
#define false 0

#define VK_CHECK(call)                 \
    do {                               \
        VkResult result_ = (call);     \
        assert(result_ == VK_SUCCESS); \
    } while (0)

#define ARRAY_SIZE(array) sizeof((array)) / sizeof((array)[0])

#define KB(bytes) bytes * 1024
#define MB(bytes) bytes * 1024 * 1024
#define GB(bytes) bytes * 1024 * 1024 * 1024

#endif /* __BASE__ */
