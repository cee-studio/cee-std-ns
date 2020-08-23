#ifdef CEE_AMALGAMATION
#undef   S
#define  S(f)  _cee_boxed_##f
#else
#define  S(f)  _##f
#include "cee.hpp"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "cee-internal.h"
#endif
#include "cee-header.h"

namespace cee {
  namespace boxed {

struct S(header) {
  enum primitive_type type;
  struct sect cs;
  union primitive_value _[1];
};

static void S(trace) (void * v, enum trace_action sa) {
  struct S(header) * m = FIND_HEADER(v);
  if (sa == trace_del)
    free(m);
}

static int S(cmp) (void * v1, void * v2) {
  struct S(header) * h1 = FIND_HEADER(v1);
  struct S(header) * h2 = FIND_HEADER(v2);
  if (h1->cs.trace == h2->cs.trace)
    segfault();
  else
    segfault();
}


static struct S(header) * S(mk_header)(state::data * s, enum primitive_type t) {
  size_t mem_block_size = sizeof(struct S(header));
  struct S(header) * b = (struct S(header) *)malloc(mem_block_size);
  ZERO_CEE_SECT(&b->cs);
  b->cs.trace = S(trace);
  b->cs.resize_method = resize_with_identity;
  b->cs.mem_block_size = mem_block_size;
  b->cs.cmp = NULL;
  b->cs.n_product = 0;
  b->cs.state = s;
  b->type = t;
  b->_[0].u64 = 0;
  return b;
}

static int S(cmp_double)(double v1, double v2) {
  if (v1 == v2)
    return 0;
  else if (v1 > v2)
    return 1;
  else 
    return -1;
}

boxed::data * from_double (state::data * s, double d) {
  size_t mem_block_size = sizeof(boxed::data);
  struct S(header) * b = S(mk_header)(s, primitive_f64);
  b->cs.cmp = (void *)S(cmp_double);
  b->_[0].f64 = d;
  return (boxed::data *)b->_;
}

static int S(cmp_float)(float v1, float v2) {
  if (v1 == v2)
    return 0;
  else if (v1 > v2)
    return 1;
  else 
    return -1;
}

boxed::data * from_float (state::data * s, float d) {
  size_t mem_block_size = sizeof(boxed::data);
  struct S(header) * b = S(mk_header)(s, primitive_f32);
  b->cs.cmp = (void *)S(cmp_float);
  b->_[0].f32 = d;
  return (boxed::data *)b->_;
}

static int S(cmp_u64)(uint64_t v1, uint64_t v2) {
  if (v1 == v2)
    return 0;
  else if (v1 > v2)
    return 1;
  else 
    return -1;
}

boxed::data * from_u64 (state::data * s, uint64_t d) {
  size_t mem_block_size = sizeof(boxed::data);
  struct S(header) * b = S(mk_header)(s, primitive_u64);
  b->_[0].u64 = d;
  return (boxed::data *)b->_;
}

static int S(cmp_u32)(uint32_t v1, uint32_t v2) {
  if (v1 == v2)
    return 0;
  else if (v1 > v2)
    return 1;
  else 
    return -1;
}

boxed::data * from_u32 (state::data * s, uint32_t d) {
  size_t mem_block_size = sizeof(boxed::data);
  struct S(header) * b = S(mk_header)(s, primitive_u32);
  b->cs.cmp = (void *)S(cmp_u32);
  b->_[0].u32 = d;
  return (boxed::data *)b->_;
}


static int S(cmp_u16)(uint16_t v1, uint16_t v2) {
  if (v1 == v2)
    return 0;
  else if (v1 > v2)
    return 1;
  else 
    return -1;
}

boxed::data * from_u16 (state::data * s, uint16_t d) {
  size_t mem_block_size = sizeof(boxed::data);
  struct S(header) * b = S(mk_header)(s, primitive_u16);
  b->cs.cmp = (void *) S(cmp_u16);
  b->_[0].u16 = d;
  return (boxed::data *)b->_;
}


static int S(cmp_u8)(uint8_t v1, uint8_t v2) {
  if (v1 == v2)
    return 0;
  else if (v1 > v2)
    return 1;
  else 
    return -1;
}

boxed::data * from_u8 (state::data * s, uint8_t d) {
  size_t mem_block_size = sizeof(boxed::data);
  struct S(header) * b = S(mk_header)(s, primitive_u8);
  b->cs.cmp = (void *)S(cmp_u8);
  b->_[0].u8 = d;
  return (boxed::data *)b->_;
}


static int S(cmp_i64)(int64_t v1, int64_t v2) {
  if (v1 == v2)
    return 0;
  else if (v1 > v2)
    return 1;
  else 
    return -1;
}

boxed::data * from_i64 (state::data *s, int64_t d) {
  size_t mem_block_size = sizeof(boxed::data);
  struct S(header) * b = S(mk_header)(s, primitive_i64);
  b->cs.cmp = (void *)S(cmp_i64);
  b->_[0].i64 = d;
  return (boxed::data *)b->_;
}

static int S(cmp_i32)(int32_t v1, int32_t v2) {
  if (v1 == v2)
    return 0;
  else if (v1 > v2)
    return 1;
  else 
    return -1;
}

boxed::data * from_i32 (state::data * s, int32_t d) {
  size_t mem_block_size = sizeof(boxed::data);
  struct S(header) * b = S(mk_header)(s, primitive_i32);
  b->cs.cmp = (void *)S(cmp_i32);
  b->_[0].i32 = d;
  return (boxed::data *)b->_;
}

static int S(cmp_i16)(int16_t v1, int16_t v2) {
  if (v1 == v2)
    return 0;
  else if (v1 > v2)
    return 1;
  else 
    return -1;
}

boxed::data * from_i16 (state::data * s, int16_t d) {
  size_t mem_block_size = sizeof(boxed::data);
  struct S(header) * b = S(mk_header)(s, primitive_i16);
  b->cs.cmp = (void *)S(cmp_i16);
  b->_[0].i16 = d;
  return (boxed::data *)b->_;
}

static int S(cmp_i8)(int8_t v1, int8_t v2) {
  if (v1 == v2)
    return 0;
  else if (v1 > v2)
    return 1;
  else 
    return -1;
}

boxed::data * from_i8 (state::data *s, int8_t d) {
  size_t mem_block_size = sizeof(boxed::data);
  struct S(header) * b = S(mk_header)(s, primitive_i8);
  b->cs.cmp = (void *)S(cmp_i8);
  b->_[0].i8 = d;
  return (boxed::data *)b->_;
}

size_t snprint (char * buf, size_t size, boxed::data * x) {
  struct S(header) * h = FIND_HEADER(x);
  int s;
  switch(h->type)
  {
    case primitive_f64:
      s = ::snprintf(buf, size, "%lf", h->_[0].f64);
      break;
    case primitive_f32:
      s = ::snprintf(buf, size, "%f", h->_[0].f32);
      break;
    case primitive_i64:
      s = ::snprintf(buf, size, "%lld", h->_[0].i64);
      break;
    case primitive_u32:
    case primitive_u16:
    case primitive_u8:
      s = ::snprintf(buf, size, "%u", h->_[0].u32);
      break;
    case primitive_u64:
      s = ::snprintf(buf, size, "%llu", h->_[0].u64);
      break;
    case primitive_i32:
    case primitive_i16:
    case primitive_i8:
      s = ::snprintf(buf, size, "%d", h->_[0].i8);
      break;
    default:
      segfault();
      break;
  }
  if (s > 0)
    return (size_t)s;
  else
    segfault();
}

double to_double (boxed::data * x) {
  struct S(header) * h = FIND_HEADER(x);
  if (h->type == primitive_f64)
    return h->_[0].f64;
  else
    segfault();
}

float to_float (boxed::data * x) {
  struct S(header) * h = FIND_HEADER(x);
  if (h->type == primitive_f32)
    return h->_[0].f32;
  else
    segfault();
}

uint64_t to_u64 (boxed::data * x) {
  struct S(header) * h = FIND_HEADER(x);
  if (h->type == primitive_u64)
    return h->_[0].u64;
  else
    segfault();
}

uint32_t to_u32 (boxed::data * x) {
  struct S(header) * h = FIND_HEADER(x);
  if (h->type == primitive_u32)
    return h->_[0].u32;
  else
    segfault();
}

uint16_t to_u16 (boxed::data * x) {
  struct S(header) * h = FIND_HEADER(x);
  if (h->type == primitive_u16)
    return h->_[0].u16;
  else
    segfault();
}

uint8_t to_u8 (boxed::data * x) {
  struct S(header) * h = FIND_HEADER(x);
  if (h->type == primitive_u8)
    return h->_[0].u8;
  else
    segfault();
}


int64_t to_i64 (boxed::data * x) {
  struct S(header) * h = FIND_HEADER(x);
  if (h->type == primitive_i64)
    return h->_[0].i64;
  else
    segfault();
}

int32_t to_i32 (boxed::data * x) {
  struct S(header) * h = FIND_HEADER(x);
  if (h->type == primitive_i32)
    return h->_[0].i32;
  else
    segfault();
}

int16_t to_i16 (boxed::data * x) {
  struct S(header) * h = FIND_HEADER(x);
  if (h->type == primitive_i16)
    return h->_[0].i16;
  else
    segfault();
}

int8_t to_i8 (boxed::data * x) {
  struct S(header) * h = FIND_HEADER(x);
  if (h->type == primitive_i8)
    return h->_[0].i8;
  else
    segfault();
}

  }
}