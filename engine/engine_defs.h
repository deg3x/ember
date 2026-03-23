#ifndef ENGINE_DEFS_H
#define ENGINE_DEFS_H

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef i8       b8;
typedef i16      b16;
typedef i32      b32;
typedef i64      b64;

typedef float    f32;
typedef double   f64;

typedef char     c8;

#define U8_MAX  UINT8_MAX
#define U16_MAX UINT16_MAX
#define U32_MAX UINT32_MAX
#define U64_MAX UINT64_MAX

#define U8_MIN  UINT8_MIN
#define U16_MIN UINT16_MIN
#define U32_MIN UINT32_MIN
#define U64_MIN UINT64_MIN

#define I8_MAX  INT8_MAX
#define I16_MAX INT16_MAX
#define I32_MAX INT32_MAX
#define I64_MAX INT64_MAX

#define I8_MIN  INT8_MIN
#define I16_MIN INT16_MIN
#define I32_MIN INT32_MIN
#define I64_MIN INT64_MIN

#define U32_FROM_U64_CLAMPED(v) (((v) > U32_MAX) ? U32_MAX : (u32)(v))

#define EMBER_TRUE  1
#define EMBER_FALSE 0

#define KB(n)    (((u64)n) << 10)
#define MB(n)    (((u64)n) << 20)
#define GB(n)    (((u64)n) << 30)
#define TB(n)    (((u64)n) << 40)
#define TO_KB(n) (((u64)n) >> 10)
#define TO_MB(n) (((u64)n) >> 20)
#define TO_GB(n) (((u64)n) >> 30)
#define TO_TB(n) (((u64)n) >> 40)

#define ARRAY_COUNT(a) (sizeof(a) / sizeof(a[0]))

#define MEMORY_ZERO(p, s) memset((p), 0, (s));

#define IS_WHITESPACE(c) ((c) == 0x20 || (c) == 0x12 || (c) == 0x09 || (c) == 0x0d || (c) == 0x0b || (c) == 0x0c)

#if defined(_MSC_VER)
    #define ALIGN_OF(t) __alignof(t)
#else
    #define ALIGN_OF(t) MAX(sizeof(t), 8)
#endif

#define global   static
#define internal static
#define persist  static

#define ARENA_FREE_LIST 1

#if defined(_MSC_VER)
    #define DEBUG_BREAK() __debugbreak()
#elif defined(__clang__) || defined(__gcc__)
    #define DEBUG_BREAK() __builtin_trap()
#else
    #define DEBUG_BREAK()
#endif

#if EMBER_PROFILING_ENABLED

#include <intrin.h>

typedef struct trace_event trace_event;
struct trace_event
{
    f64 time_diff;
    u64 cpu_clock_start;
    u64 cpu_clock_end;
    u64 cpu_clock_diff;
};

#define EMBER_TRACE_BEGIN(n)                    \
        trace_event _event_##n;                 \
        platform_timer _timer_##n;              \
        _timer_##n = platform_timer_init();     \
        _event_##n.cpu_clock_start = __rdtsc()

#define EMBER_TRACE_END(n)                                                  \
        platform_timer_update(&_timer_##n);                                 \
        _event_##n.cpu_clock_end  = __rdtsc();                              \
        _event_##n.time_diff      = platform_timer_since_start(_timer_##n); \
        _event_##n.cpu_clock_diff = _event_##n.cpu_clock_end - _event_##n.cpu_clock_start

#else

#define EMBER_TRACE_BEGIN(n)
#define EMBER_TRACE_END(n)

#endif // EMBER_PROFILING_ENABLED

#if EMBER_ASSERT_ENABLED
void assert_fail(const c8* expression, const c8* message, const c8* file, i32 line)
{
#if EMBER_BUILD_CONSOLE
    fprintf(stderr, "Assertion failed: (%s)\n\nFile: [%s::%d]\nMessage: %s\n", expression, file, line, message);
#endif

#if PLATFORM_WINDOWS
    c8 assert_buf[4096];

    wsprintfA(assert_buf, "Assertion failed: (%s)\n\nFile: [%s::%d]\nMessage: %s\n", expression, file, line, message);

    MessageBoxA(NULL, assert_buf, "Assertion Failure", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
#endif

    return;
}

#define EMBER_ASSERT(expr)                                  \
        {                                                   \
            if (expr) {}                                    \
            else                                            \
            {                                               \
                assert_fail(#expr, "", __FILE__, __LINE__); \
                DEBUG_BREAK();                              \
            }                                               \
        }

#define EMBER_ASSERT_MSG(expr, msg)                          \
        {                                                    \
            if (expr) {}                                     \
            else                                             \
            {                                                \
                assert_fail(#expr, msg, __FILE__, __LINE__); \
                DEBUG_BREAK();                               \
            }                                                \
        }

#else
#define EMBER_ASSERT(expr)
#define EMBER_ASSERT_MSG(expr, msg)
#endif // EMBER_ASSERT_ENABLED

#endif // ENGINE_DEFS_H
