#ifdef CEE_AMALGAMATION
#undef   S
#define  S(f)  _cee_env_##f
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
  namespace env {

struct S(header) {
  enum del_policy env_dp;
  enum del_policy vars_dp;
  struct sect cs;
  struct data _;
};
    
#include "cee-resize.h"
    
static void S(trace) (void * v, enum trace_action ta) {
  struct S(header) * h = FIND_HEADER(v);
  switch (ta) {
    case trace_del_no_follow:
      S(de_chain)(h);
      free(h);
      break;
    case trace_del_follow:
      del_e(h->env_dp, h->_.outer);
      del_e(h->vars_dp, h->_.vars);
      S(de_chain)(h);
      free(h);
      break;
    default:
      h->cs.gc_mark = ta - trace_mark;
      trace(h->_.outer, ta);
      trace(h->_.vars, ta);
      break;
  }
}

env::data * mk_e(state::data * st, enum del_policy dp[2], env::data * outer, map::data * vars) {
  size_t mem_block_size = sizeof(struct S(header));
  struct S(header) * h = (struct S(header) *)malloc(mem_block_size);

  ZERO_CEE_SECT(&h->cs);
  S(chain)(h, st);
  
  h->cs.trace = S(trace);
  h->cs.resize_method = resize_with_identity;
  h->cs.mem_block_size = mem_block_size;
  h->cs.cmp = NULL;
  h->cs.n_product = 0;
  
  h->env_dp = dp[0];
  h->vars_dp = dp[1];
  h->_.outer = outer;
  h->_.vars = vars;
  
  return &h->_;
}
 
env::data * mk(state::data * st, env::data * outer, map::data * vars) {
  enum del_policy dp[2] = { CEE_DEFAULT_DEL_POLICY, CEE_DEFAULT_DEL_POLICY };
  return mk_e (st, dp, outer, vars);
}
  
  
    
  } // env
} // cee