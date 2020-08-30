#ifdef CEE_AMALGAMATION
#undef  S
#define S(f)  _cee_map_##f
#else
#define S(f)  _##f
#include "cee.hpp"
#include "cee-internal.h"
#include <stdlib.h>
#include <string.h>
#endif
#include "cee-header.h"

namespace cee {
  namespace map {

struct S(header) {
  void * context;
  int (*cmp)(const void *l, const void *r);
  uintptr_t size;
  enum del_policy key_del_policy;
  enum del_policy val_del_policy;
  enum trace_action ta;
  struct sect cs;
  void * _[1];
};
    
#include "cee-resize.h"

static void S(free_pair_follow)(void * cxt, void * c) {
  del(c);
}
        
static void S(trace_pair) (void * cxt, const void *nodep, const VISIT which, const int depth) {
  tuple::data * p;
  struct S(header) * h;
  switch (which) 
  {
    case preorder:
    case leaf:
      p = (tuple::data *)*(void **)nodep;
      trace(p, *(enum trace_action *)cxt);
      break;
    default:
      break;
  }
}

static void S(trace)(void * p, enum trace_action ta) {
  struct S(header) * h = FIND_HEADER (p);
  switch (ta) {
    case trace_del_no_follow:
      musl_tdestroy(NULL, h->_[0], NULL);
      S(de_chain)(h);
      free(h);
      break;
    case trace_del_follow:
      musl_tdestroy((void *)&ta, h->_[0], S(free_pair_follow));
      S(de_chain)(h);
      free(h);
      break;
    default:
      h->cs.gc_mark = ta - trace_mark;
      h->ta = ta;
      musl_twalk(&ta, h->_[0], S(trace_pair));
      break;
  }
}

static int S(cmp) (void * cxt, const void * v1, const void * v2) {
  struct S(header) * h = (struct S(header) *) cxt;
  tuple::data * t1 = (tuple::data *) v1;
  tuple::data * t2 = (tuple::data *) v2;
  return h->cmp(t1->_[0], t2->_[0]);
}

map::data * mk_e (state::data * st, enum del_policy o[2], 
                  int (*cmp)(const void *, const void *)) {
  size_t mem_block_size = sizeof(struct S(header));
  struct S(header) * m = (struct S(header) *)malloc(mem_block_size);
  m->context = NULL;
  m->cmp = cmp;
  m->size = 0;
  ZERO_CEE_SECT(&m->cs);
  S(chain)(m, st);
  
  m->cs.trace = S(trace);
  m->cs.resize_method = resize_with_identity;
  m->cs.mem_block_size = mem_block_size;
  m->cs.cmp = 0;
  m->cs.cmp_stop_at_null = 0;
  m->cs.n_product = 2; // key, value
  
  m->key_del_policy = o[0];
  m->val_del_policy = o[1];
  m->_[0] = 0;
  return (map::data *)m->_;
}

map::data * mk(state::data * st, int (*cmp) (const void *, const void *)) {
  static enum del_policy d[2] = { CEE_DEFAULT_DEL_POLICY, CEE_DEFAULT_DEL_POLICY };
  return mk_e(st, d, cmp);
}
    
uintptr_t size(struct map::data * m) {
  struct S(header) * b = FIND_HEADER(m);
  return b->size;
}

void add(map::data * m, void * key, void * value) {
  struct S(header) * b = FIND_HEADER(m);
  
  enum del_policy d[2];
  d[0] = b->key_del_policy;
  d[1] = b->val_del_policy;
  
  tuple::data * t = tuple::mk_e(b->cs.state, d, key, value);
  tuple::data ** oldp = (tuple::data **)musl_tsearch(b, t, b->_, S(cmp));
  if (oldp == NULL)
    segfault(); // run out of memory
  else if (*oldp != t) 
    del(t);
  else
    b->size ++;
  return;
}

void * find(map::data * m, void * key) {
  struct S(header) * b = FIND_HEADER(m);
  tuple::data t = { key, 0 };
  void **oldp = (void **)musl_tfind(b, &t, b->_, S(cmp));
  if (oldp == NULL)
    return NULL;
  else {
    tuple::data * p = (tuple::data *)*oldp;
    return p->_[1];
  }
}

void * remove(map::data * m, void * key) {
  struct S(header) * b = FIND_HEADER(m);
  void ** oldp = (void **)musl_tdelete(b, key, b->_, S(cmp));
  if (oldp == NULL)
    return NULL;
  else {
    b->size --;
    tuple::data * ret = (tuple::data *)*oldp;
    del(ret);
    decr_indegree(b->key_del_policy, ret->_[0]);
    decr_indegree(b->val_del_policy, ret->_[1]);
    return ret->_[1];
  }
}

static void S(get_key) (void * cxt, const void *nodep, const VISIT which, const int depth) {
  tuple::data * p;
  switch (which) 
  {
    case preorder:
    case leaf:
      p = *(tuple::data **)nodep;
      list::append((list::data **)cxt, p->_[0]);
      break;
    default:
      break;
  }
}

list::data * keys(map::data * m) {
  uintptr_t n = map::size(m);
  struct S(header) * b = FIND_HEADER(m);
  list::data * keys = list::mk(b->cs.state, n);
  b->context = keys;
  musl_twalk(&keys, b->_[0], S(get_key));
  return keys;
}


static void S(get_value) (void * cxt, const void *nodep, const VISIT which, const int depth) {
  tuple::data  * p;
  switch (which) 
  {
    case preorder:
    case leaf:
      p = (tuple::data *)*(void **)nodep;
      list::append((list::data **)cxt, p->_[1]);
      break;
    default:
      break;
  }
}

list::data * values(map::data * m) {
  uintptr_t s = map::size(m);
  struct S(header) * b = FIND_HEADER(m);
  list::data * values = list::mk(b->cs.state, s);
  b->context = values;
  musl_twalk(&values, b->_[0], S(get_value));
  return values;
}
  
  }
}