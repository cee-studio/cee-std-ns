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
  struct sect cs;
  void * _[1];
};

struct S(pair) {
  tuple::data * value;
  struct S(header) * h;
};

static void S(free_pair)(void * c) {
  struct S(pair) * p = (struct S(pair) *)c;
  del(p->value);
  free(p);
}

static void S(del)(void * p) {
  struct S(header) * b = FIND_HEADER (p);
  tdestroy(b->_[0], S(free_pair));
  free(b);
}

static int S(cmp) (const void * v1, const void * v2) {
  struct S(pair) * t1 = (struct S(pair) *) v1;
  struct S(pair) * t2 = (struct S(pair) *) v2;
  if (t1->h == t2->h)
  	return t1->h->cmp(t1->value->_[0], t2->value->_[0]);
  else
    segfault();
}

map::data * mk_e (enum del_policy o[2], int (*cmp)(const void *, const void *)) {
  size_t mem_block_size = sizeof(struct S(header));
  struct S(header) * m = (struct S(header) *)malloc(mem_block_size);
  m->context = NULL;
  m->cmp = cmp;
  m->size = 0;
  ZERO_CEE_SECT(&m->cs);
  m->cs.del = S(del);
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

map::data * mk(int (*cmp) (const void *, const void *)) {
  static enum del_policy d[2] = { CEE_DEFAULT_DEL_POLICY, CEE_DEFAULT_DEL_POLICY };
  return mk_e(d, cmp);
}
    
uintptr_t size(struct map::data * m) {
  struct S(header) * b = FIND_HEADER(m);
  return b->size;
}

void add(map::data * m, void * key, void * value) {
  struct S(header) * b = FIND_HEADER(m);
  struct S(pair) * triple = (struct S(pair) *) malloc(sizeof(struct S(pair)));
  triple->h = b;
  
  enum del_policy d[2];
  d[0] = b->key_del_policy;
  d[1] = b->val_del_policy;
  
  triple->value = tuple::mk_e(d, key, value);
  struct S(pair) ** oldp = (struct S(pair) **)tsearch(triple, b->_, S(cmp));
  if (oldp == NULL)
    segfault(); // run out of memory
  else if (*oldp != triple) 
    S(free_pair)(triple);
  else
    b->size ++;
  return;
}

void * find(map::data * m, void * key) {
  struct S(header) * b = FIND_HEADER(m);
  tuple::data t = { key, 0 };
  struct S(pair) keyp = { .value = &t, .h = b };
  void **oldp = (void **)tfind(&keyp, b->_, S(cmp));
  if (oldp == NULL)
    return NULL;
  else {
    struct S(pair) * p = (struct S(pair) *)*oldp;
    return p->value->_[1];
  }
}

void * remove(map::data * m, void * key) {
  struct S(header) * b = FIND_HEADER(m);
  void ** oldp = (void **)tdelete(key, b->_, S(cmp));
  if (oldp == NULL)
    return NULL;
  else {
    b->size --;
    struct S(pair) * t = (struct S(pair) *)*oldp;
    tuple::data * ret = t->value;
    S(free_pair)(t);
    decr_indegree(b->key_del_policy, ret->_[0]);
    decr_indegree(b->val_del_policy, ret->_[1]);
    return ret->_[1];
  }
}

static void S(get_key) (const void *nodep, const VISIT which, const int depth) {
  struct S(pair) * p;
  struct S(header) * h;
  array::data * keys;
  switch (which) 
  {
    case preorder:
    case leaf:
      p = *(struct S(pair) **)nodep;
      h = p->h;
      keys = (array::data *)h->context;
      h->context = array::append(keys, p->value->_[0]);
      break;
    default:
      break;
  }
}

array::data * keys(map::data * m) {
  uintptr_t s = map::size(m);
  struct S(header) * b = FIND_HEADER(m);
  array::data * keys = array::mk(s);
  b->context = keys;
  twalk(b->_[0], S(get_key));
  return keys;
}


static void S(get_value) (const void *nodep, const VISIT which, const int depth) {
  struct S(pair) * p;
  struct S(header) * h;
  array::data * values;
  switch (which) 
  {
    case preorder:
    case leaf:
      p = (struct S(pair) *)*(void **)nodep;
      h = p->h;
      values = (array::data *)h->context;
      h->context = array::append(values, p->value->_[1]);
      break;
    default:
      break;
  }
}

array::data * values(map::data * m) {
  uintptr_t s = map::size(m);
  struct S(header) * b = FIND_HEADER(m);
  array::data * values = array::mk(s);
  b->context = values;
  twalk(b->_[0], S(get_value));
  return values;
}
  
  }
}