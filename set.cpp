#ifdef CEE_AMALGAMATION
#undef  S
#define S(f)    _cee_set_##f
#else
#define S(f)    _##f
#include "cee.hpp"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "cee-internal.h"
#endif
#include "cee-header.h"

namespace cee {
  namespace set {

struct S(header) {
  void * context;
  int (*cmp)(const void *l, const void *r);
  uintptr_t size;
  enum del_policy del_policy;
  enum trace_action ta;
  struct sect cs;
  void * _[1];
};

#include "cee-resize.h"

static void S(free_pair_follow) (void * cxt, void * c) {
  enum del_policy dp = * (enum del_policy *) cxt;
  del_e(dp, c);
}
    
static void S(trace_pair) (void * cxt, const void *nodep, const VISIT which, const int depth) {
  void * p;
  struct S(header) * h;
  switch (which) 
  {
    case preorder:
    case leaf:
      p = *(void **)nodep;
      trace(p, *((enum trace_action *)cxt));
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
      musl_tdestroy(NULL, h->_[0], S(free_pair_follow));
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


/*
 * create a new set and the equality of 
 * its two elements are decided by cmp
 * dt: specify how its elements should be handled if the set is deleted.
 */
set::data * mk_e (state::data * st, enum del_policy o, 
                  int (*cmp)(const void *, const void *)) 
{
  struct S(header) * m = (struct S(header) *)malloc(sizeof(struct S(header)));
  m->cmp = cmp;
  m->size = 0;
  ZERO_CEE_SECT(&m->cs);
  S(chain)(m, st);
  
  m->cs.trace = S(trace);
  m->cs.resize_method = resize_with_identity;
  m->cs.n_product = 1;
  m->context = NULL;
  m->_[0] = NULL;
  m->del_policy = o;
  return (set::data *)m->_;
}

set::data * mk (state::data * s, int (*cmp)(const void *, const void *)) {
  return set::mk_e(s, CEE_DEFAULT_DEL_POLICY, cmp);
}

size_t size (set::data * s) {
  struct S(header) * h = FIND_HEADER(s);
  return h->size;
}

bool empty (set::data * s) {
  struct S(header) * h = FIND_HEADER(s);
  return h->size == 0;
}

/*
 * add an element key to the set m
 * 
 */
void add(set::data *m, void * val) {
  struct S(header) * h = FIND_HEADER(m);
  void ** oldp = (void **) musl_tsearch(val, h->_, h->cmp);
  
  if (oldp == NULL)
    segfault();
  else if (*oldp != (void *)val)
    return;
  else {
    h->size ++;
    incr_indegree(h->del_policy, val);
  }
  return;
}
    
static void S(del)(void * cxt, void * p) {
  enum del_policy dp = *((enum del_policy *)cxt);
  switch(dp) {
    case dp_del_rc:
      del_ref(p);
      break;  
    case dp_del:
      del(p);
      break;
    case dp_noop:
      break;
  }
}    
void cee_set_clear (set::data * s) {
  struct S(header) * h = FIND_HEADER (s);
  musl_tdestroy(&h->del_policy, h->_[0], S(del));
  h->_[0] = NULL;
  h->size = 0;
}

void * find(set::data *m, void * key) {
  struct S(header) * h = FIND_HEADER(m);
  void *oldp = (void *) musl_tfind(key, h->_, h->cmp);
  if (oldp == NULL)
    return NULL;
  else
    return oldp;
}

static void S(get_value) (void * cxt, const void *nodep, const VISIT which, const int depth) {
  void * p;
  switch (which) 
  {
    case preorder:
    case leaf:
      p = *(void **)nodep;
      list::append((list::data **)cxt, p);
      break;
    default:
      break;
  }
}

list::data * values(set::data * m) {
  uintptr_t s = set::size(m);
  struct S(header) * h = FIND_HEADER(m);
  h->context = list::mk(h->cs.state, s);
  use_realloc(h->context);
  musl_twalk(&h->context, h->_[0], S(get_value));
  return (list::data *)h->context;
}

void * remove(set::data *m, void * key) {
  struct S(header) * h = FIND_HEADER(m);
  void ** old = (void **)musl_tfind(key, h->_, h->cmp);
  if (old == NULL)
    return NULL;
  else {
    h->size --;
    void * k = *old;
    musl_tdelete(key, h->_, h->cmp);
    return k;
  }
}

set::data * union_set (state::data * s, set::data * s1, set::data * s2) {
  struct S(header) * h1 = FIND_HEADER(s1);
  struct S(header) * h2 = FIND_HEADER(s2);
  if (h1->cmp == h2->cmp) {
    set::data * s0 = set::mk(s, h1->cmp);
    list::data * v1 = set::values(s1);
    list::data * v2 = set::values(s2);
    int i;
    for (i = 0; i < list::size(v1); i++)
      set::add(s0, v1->_[i]);
    
    for (i = 0; i < list::size(v2); i++)
      set::add(s0, v2->_[i]);
    
    del(v1);
    del(v2);
    return s0;
  } else
    segfault();
  return NULL;
}
    
  }
}