#ifdef CEE_AMALGAMATION
#undef   S
#define  S(f)  _cee_n_tuple_##f
#else
#define  S(f)  _##f
#include "cee.hpp"
#include "cee-internal.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#endif
#include "cee-header.h"

#define CEE_MAX_N_TUPLE  16

namespace cee {
  namespace n_tuple {
  
struct S(header) {
  enum del_policy del_policies[CEE_MAX_N_TUPLE];
  struct sect cs;
  void * _[CEE_MAX_N_TUPLE];
};
    
#include "cee-resize.h"
    
static void S(trace)(void * v, enum trace_action ta) {
  struct S(header) * b = FIND_HEADER(v);
  int i; 
  
  switch (ta) {
    case trace_del_no_follow:
      S(de_chain)(b);
      free(b);
      break;
    case trace_del_follow:
      for (i = 0; i < b->cs.n_product; i++)
        del_e(b->del_policies[i], b->_[i]);
      
      S(de_chain)(b);
      free(b);
      break;
    default:
      b->cs.gc_mark = ta - trace_mark;
      for (i = 0; i < b->cs.n_product; i++)
        trace(b->_[i], ta);
      break;
  }
}

static struct S(header) * cee_n_tuple_v (state::data * st, size_t ntuple, 
                                         enum del_policy o[], va_list ap) {
  if (ntuple > CEE_MAX_N_TUPLE)
    segfault();
  
  size_t mem_block_size = sizeof(struct S(header));
  struct S(header) * m = (struct S(header) *) malloc(mem_block_size);
  ZERO_CEE_SECT(&m->cs);
  S(chain)(m, st);
  
  m->cs.trace = S(trace);
  m->cs.resize_method = resize_with_identity;
  m->cs.mem_block_size = mem_block_size;
  m->cs.n_product = ntuple;
  
  int i;
  for(i = 0; i < ntuple; i++) {
    m->_[i] = va_arg(ap, void *);
    m->del_policies[i] = o[i];
    incr_indegree(o[i], m->_[i]);
  }
  return m;
}

n_tuple::data * mk (state::data * st, size_t ntuple, ...) {
  va_list ap;
  va_start(ap, ntuple);
  enum del_policy * o = (enum del_policy *)malloc(ntuple * sizeof (enum del_policy));
  int i;
  for (i = 0; i < ntuple; i++)
    o[i] = CEE_DEFAULT_DEL_POLICY;
  
  struct S(header) * h = cee_n_tuple_v(st, ntuple, o, ap);
  free(o);
  return (n_tuple::data *)(h->_);
}

  }
}