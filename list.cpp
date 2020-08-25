#ifdef CEE_AMALGAMATION
#undef   S
#define  S(f)  _cee_list_##f
#else
#define  S(f)  _##f
#include "cee.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "cee-internal.h"
#endif
#include "cee-header.h"

namespace cee {
namespace list {

struct S(header) {
  uintptr_t size;
  uintptr_t capacity;
  enum del_policy del_policy;
  struct sect cs;
  void * _[];
};

#include "cee-resize.h"

static void S(trace) (void * v, enum trace_action ta) {
  struct S(header) * m = FIND_HEADER(v);
  int i;
  
  switch(ta) {
    case trace_del_no_follow:
      S(de_chain)(m);
      free(m);
      break;
    case trace_del_follow:
      for (i = 0; i < m->size; i++)
        del_e(m->del_policy, m->_[i]);
      S(de_chain)(m);
      free(m);
      break;
    default:
      m->cs.gc_mark = ta - trace_mark;
      for (i = 0; i < m->size; i++)
        trace(m->_[i], ta);
      break;
  }
}

list::data * mk_e (state::data * st, enum del_policy o, size_t cap) {
  size_t mem_block_size = sizeof(struct S(header)) + cap * sizeof(void *);
  struct S(header) * m = (struct S(header) *)malloc(mem_block_size);
  m->capacity = cap;
  m->size = 0;
  m->del_policy = o;
  ZERO_CEE_SECT(&m->cs);
  S(chain)(m, st);
  
  m->cs.trace = S(trace);
  m->cs.resize_method = resize_with_malloc;
  m->cs.mem_block_size = mem_block_size;
  
  return (list::data *)(m->_);
}

list::data * mk (state::data * s, size_t cap) {
  return mk_e(s, CEE_DEFAULT_DEL_POLICY, cap);
}

list::data * append (list::data ** l, void *e) {
  list::data * v = *l;
    
  struct S(header) * m = FIND_HEADER(v);
  size_t capacity = m->capacity;
  size_t extra_cap = capacity ? capacity : 1;
  if (m->size == m->capacity) {
    size_t new_mem_block_size = m->cs.mem_block_size + extra_cap * sizeof(void *);
    struct S(header) * m1 = S(resize)(m, new_mem_block_size);
    m1->capacity = capacity + extra_cap;
    *l = (list::data *)m1->_;
    m = m1;
  }
  m->_[m->size] = e;
  m->size ++;
  incr_indegree(m->del_policy, e);
  return *l;
}

list::data * insert(state::data * s, list::data ** l, size_t index, void *e) {
  list::data * v = *l;
  if (v == NULL) {
    v = mk(s, 10);
    use_realloc(v);
  }
  
  struct S(header) * m = FIND_HEADER(v);
  size_t capacity = m->capacity;
  size_t extra_cap = capacity ? capacity : 1;
  if (m->size == m->capacity) {
    size_t new_mem_block_size = m->cs.mem_block_size + extra_cap * sizeof(void *);
    struct S(header) * m1 = S(resize)(m, new_mem_block_size);
    m1->capacity = capacity + extra_cap;
    *l = (list::data *)m1->_;
    m = m1;
  }
  int i;
  for (i = m->size; i > index; i--)
    m->_[i] = m->_[i-1];
  
  m->_[index] = e;
  m->size ++;
  incr_indegree(m->del_policy, e);
  return *l;
}

bool remove(list::data * v, size_t index) {
  struct S(header) * m = FIND_HEADER(v);
  if (index >= m->size) return false;
 
  void * e = m->_[index];
  m->_[index] = 0;
  int i;
  for (i = index; i < (m->size - 1); i++)
    m->_[i] = m->_[i+1];
  
  m->size --;
  decr_indegree(m->del_policy, e);
  return true;
}


size_t size (list::data *x) {
  struct S(header) * m = FIND_HEADER(x);
  return m->size;
}

size_t capacity (list::data * x) {
  struct S(header) * h = FIND_HEADER(x);
  return h->capacity;
}
  
  }
}