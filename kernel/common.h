#pragma once

#define static_assert _Static_assert

typedef signed char    i8;
typedef short          i16;
typedef int            i32;
typedef long           i64;
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef unsigned long  u64;

static_assert(sizeof(i8)  == 1, "sizeof(i8) != 1");
static_assert(sizeof(i16) == 2, "sizeof(i16) != 2");
static_assert(sizeof(i32) == 4, "sizeof(i32) != 4");
static_assert(sizeof(i64) == 8, "sizeof(i64) != 8");
static_assert(sizeof(u8)  == 1, "sizeof(u8) != 1");
static_assert(sizeof(u16) == 2, "sizeof(u16) != 2");
static_assert(sizeof(u32) == 4, "sizeof(u32) != 4");
static_assert(sizeof(u64) == 8, "sizeof(u64) != 8");

#define NULL ((void*) 0)

typedef _Bool bool;

#define false 0
#define true  1

#define va_list       __builtin_va_list
#define va_start(v,l) __builtin_va_start(v,l)
#define va_end(v)     __builtin_va_end(v)
#define va_arg(v,l)   __builtin_va_arg(v,l)
#define va_copy(d,s)  __builtin_va_copy(d,s)

#define __unused __attribute__((unused))

#define offsetof(type, member) __builtin_offsetof(type, member)
#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))
#define abs(x) ((x) >= 0 ? (x) : (-x))

#define PAGE_SIZE 0x1000

#define ALIGNUP_PAGE(val) ((val + (PAGE_SIZE - 1)) / PAGE_SIZE * PAGE_SIZE)

void kloop(void);

