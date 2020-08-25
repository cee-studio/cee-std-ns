#ifdef CEE_AMALGAMATION
#undef   S
#define  S(f)  _cee_quadruple_##f
#else
#define  S(f)  _##f
#include "cee.hpp"
#include "cee-internal.h"
#include <stdlib.h>
#include <string.h>
#endif
#include "cee-header.h"

namespace cee {
  namespace quadruple {
    
struct S(header) {
  enum del_policy del_policies[4];
  struct sect cs;
  void * _[4];
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
      for (i = 0; i < 4; i++)
        del_e(b->del_policies[i], b->_[i]);
      S(de_chain)(b);
      free(b);
      break;
    default:
      b->cs.gc_mark = ta - trace_mark;
      for (i = 0; i < 4; i++)
        trace(b->_[i], ta);
      break;
  }
}

quadruple::data * mk_e (state::data * st, enum del_policy o[4], 
                        void * v1, void * v2, void * v3, void * v4) {
  size_t mem_block_size = sizeof(struct S(header));
  struct S(header) * m = (struct S(header) *) malloc(mem_block_size);
  ZERO_CEE_SECT(&m->cs);
  S(chain)(m, st);
  
  m->cs.trace = S(trace);
  m->cs.resize_method = resize_with_identity;
  m->cs.mem_block_size = mem_block_size;
  m->cs.n_product = 4;
  m->cs.state = st;
  m->_[0] = v1;
  m->_[1] = v2;
  m->_[2] = v3;
  m->_[3] = v4;
  int i;
  for (i = 0; i < 4; i++) {
    m->del_policies[i] = o[i];
    incr_indegree(o[i], m->_[i]);
  }
  return (quadruple::data *)&m->_;
}
  
  }
}