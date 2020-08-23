#ifdef CEE_AMALGAMATION
#undef   S
#define  S(f)  _cee_dict_##f
#else
#define  S(f)  _##f
#include "cee.hpp"
#include "cee-internal.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#endif
#include "cee-header.h"

namespace cee {
  namespace dict {

struct S(header) {
  struct list::data * keys;
  struct list::data * vals;
  uintptr_t size;
  enum del_policy del_policy;
  struct sect cs;
  struct hsearch_data _[1];
};


static void S(trace)(void *d, enum trace_action sa) {
  struct S(header) * m = FIND_HEADER(d);
  hdestroy_r(m->_);
  del_e(m->del_policy, m->keys);
  del_e(m->del_policy, m->vals);
  if (sa == trace_del)
    free(m);
}

dict::data * mk_e (state::data * s, enum del_policy o, size_t size) {
  size_t mem_block_size = sizeof(struct S(header));
  struct S(header) * m = (struct S(header) *)malloc(mem_block_size);
  m->del_policy = o;
  m->keys = list::mk(s, size);
  use_realloc(m->keys);
  
  m->vals = list::mk(s, size);
  use_realloc(m->vals);
  
  m->size = size;
  ZERO_CEE_SECT(&m->cs);
  m->cs.state = s;
  m->cs.trace = S(trace);
  m->cs.mem_block_size = mem_block_size;
  m->cs.resize_method = resize_with_identity;
  m->cs.n_product = 2; // key:str, value
  size_t  hsize = (size_t)((float)size * 1.25);
  memset(m->_, 0, sizeof(struct hsearch_data));
  if (hcreate_r(hsize, m->_))
    return (dict::data *)(m->_);
  else {
    del(m->keys);
    del(m->vals);
    free(m);
    return NULL;
  }
}

dict::data * mk (state::data *s, size_t size) {
  return dict::mk_e (s, CEE_DEFAULT_DEL_POLICY, size);
}

void add (dict::data * d, char * key, void * value) {
  struct S(header) * m = FIND_HEADER(d);
  ENTRY n, *np;
  n.key = key;
  n.data = value;
  if (!hsearch_r(n, ENTER, &np, m->_))
    segfault();
  append(&m->keys, key);
  append(&m->vals, value);
}

void * find(dict::data * d, char * key) {
  struct S(header) * m = FIND_HEADER(d);
  ENTRY n, *np;
  n.key = key;
  n.data = NULL;
  if (hsearch_r(n, FIND, &np, m->_))
    return np->data;
  printf ("%s\n", strerror(errno));
  return NULL;
}
  
  }
}