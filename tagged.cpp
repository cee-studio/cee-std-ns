#ifdef CEE_AMALGAMATION
#undef   S
#define  S(f)  _cee_tagged_##f
#else
#define  S(f)  _##f
#include "cee.hpp"
#include "cee-internal.h"
#include <stdlib.h>
#include <string.h>
#endif
#include "cee-header.h"

namespace cee {
  namespace tagged {
    
struct S(header) {
  enum del_policy del_policy;
  struct sect cs;
  struct tagged::data _;
};
    
#include "cee-resize.h"    

static void S(trace) (void * v, enum trace_action ta) {
  struct S(header) * m = FIND_HEADER(v);
  switch (ta) {
    case trace_del_no_follow:
      S(de_chain)(m);
      free(m);
      break;
    case trace_del_follow:
      del_e(m->del_policy, m->_.ptr._);
      S(de_chain)(m);
      free(m);
      break;
    default:
      m->cs.gc_mark = ta - trace_mark;
      trace(m->_.ptr._, ta);
      break;
  }
}

tagged::data * mk_e (state::data * st, enum del_policy o, uintptr_t tag, void *p) {
  size_t mem_block_size = sizeof(struct S(header));
  struct S(header) * b = (struct S(header) *)malloc(mem_block_size);
  ZERO_CEE_SECT(&b->cs);
  S(chain)(b, st);
  
  b->cs.trace = S(trace);
  b->cs.resize_method = resize_with_identity;
  b->cs.mem_block_size = mem_block_size;
  
  b->_.tag = tag;
  b->_.ptr._ = p;
  b->del_policy = o;
  incr_indegree(o, p);
  return &b->_;
}

tagged::data * mk (state::data * st, uintptr_t tag, void *p) {
  return mk_e(st, CEE_DEFAULT_DEL_POLICY, tag, p);
}
    
  }
}