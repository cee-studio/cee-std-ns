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

static void S(trace) (void * v, enum trace_action ta) {
  struct S(header) * m = FIND_HEADER(v);
  switch (ta) {
    case trace_del_follow: 
    {
      // following this tracing chain but not the relations
      struct sect * tail = m->_.trace_tail;
      while (tail != &m->cs) {
        trace(tail + 1, trace_del_no_follow);
        tail = m->_.trace_tail;
      }
      free(m);
      break;
    }
    case trace_del_no_follow: 
    {
      // TODO detach the this state from all memory blocks
      free(m);
      break;
    }
    default:
    {
      m->cs.gc_mark = ta;
      trace(m->_.roots, ta);
      break;
    }
  }
}
  
  
static void S(sweep) (void * v, enum trace_action ta) {
  struct S(header) * m = FIND_HEADER(v);
  struct sect * head  = &m->cs;
  while (head != NULL) {
    struct sect * next = head->trace_next;
    if (head->gc_mark != ta) 
      trace(head + 1, trace_del_no_follow);
    head = next;
  }
}

static int S(cmp) (const void * v1, const void * v2) {
  intptr_t u1 = (intptr_t) v1;
  intptr_t u2 = (intptr_t) v2;
  if (u1 > u2)
    return -1;
  else if (u1 == u2)
    return 0;
  else
    return -1;
}


state::data * mk() {
  size_t memblock_size = sizeof(struct S(header));
  struct S(header) * h = (struct S(header) *)malloc(memblock_size);
  ZERO_CEE_SECT(&h->cs);
  h->cs.trace = S(trace);
  h->_.trace_tail = &h->cs; // points to self;
  
  set::data * roots = set::mk_e(&h->_, dp_noop, S(cmp));
  h->_.roots = roots;
  h->_.next_mark = 1;
  return &h->_;
}  
  
void add_gc_root(state::data * s, void * key) {
  set::add(s->roots, key);
}
  
void remove_gc_root(state::data * s, void * key) {
  set::remove(s->roots, key);
}
  
void gc (state::data * s) {
  struct S(header) * h = FIND_HEADER(s);
  int mark = trace_mark + s->next_mark;
  
  trace(s, (enum trace_action)mark);
  list::data * l = set::values(s->roots);
  for (int i; i < list::size(l); i++)
    trace(l->_[i], (enum trace_action) mark);
  del(l);
  
  S(sweep)(s, (enum trace_action) mark);
  
  if (s->next_mark == 0) {
    s->next_mark = 1;
  } else {
    s->next_mark = 0;
  }
}
  
  }
}