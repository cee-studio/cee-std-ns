#ifdef CEE_AMALGAMATION
#undef   S
#define  S(f)  _cee_state_##f
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
namespace state {

struct S(header) {
  struct sect cs;
  struct data _;
};

static void S(trace) (void * v, enum trace_action sa) {
  struct S(header) * m = FIND_HEADER(v);
  if (sa == trace_del)
    free(m);
}

state::data * mk() {
  size_t memblock_size = sizeof(struct S(header));
  struct S(header) * h = (struct S(header) *)malloc(memblock_size);
  ZERO_CEE_SECT(&h->cs);
  h->cs.trace = S(trace);
  h->_.trace_dict = NULL;
  h->_.trace_env = NULL;
  h->_.trace_map = NULL;
  h->_.trace_str = NULL;
  h->_.trace_list = NULL;
  return &h->_;
}
  
  
  
  }
}