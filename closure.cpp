#ifdef CEE_AMALGAMATION
#undef   S
#define  S(f)  _cee_closure_##f
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
namespace closure {

struct S(header) {
  struct sect cs;
  struct data _;
};

static void S(trace) (void * v, enum trace_action sa) {
  struct S(header) * m = FIND_HEADER(v);
  if (sa == trace_del)
    free(m);
}

struct data * mk (state::data * s, env::data * env, void * fun) {
  size_t mem_block_size = sizeof(struct S(header));
  struct S(header) * b = (struct S(header) *)malloc(mem_block_size);
  ZERO_CEE_SECT(&b->cs);
  b->cs.trace = S(trace);
  b->cs.resize_method = resize_with_identity;
  b->cs.mem_block_size = mem_block_size;
  b->_.env = NULL;
  b->_.fun = NULL;
  return &(b->_);
}
  
} // namespace closure
} // namespace cee