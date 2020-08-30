static void S(chain) (struct S(header) * h, state::data * st) {
  h->cs.state = st;
  
  h->cs.trace_prev = st->trace_tail;
  st->trace_tail->trace_next = &h->cs;
  
  st->trace_tail = &h->cs;
}

static void S(de_chain) (struct S(header) * h) {
  state::data * st = h->cs.state;
  
  struct sect * prev = h->cs.trace_prev;
  struct sect * next = h->cs.trace_next;
  
  if (st->trace_tail == &h->cs) {
    prev->trace_next = NULL;
    st->trace_tail = prev;
  }
  else {
    prev->trace_next = next;
    if (next)
      next->trace_prev = prev;
  }
}

static struct S(header) * S(resize)(struct S(header) * h, size_t n) 
{
  state::data * state = h->cs.state;
  struct S(header) * ret;
  switch(h->cs.resize_method)
  {
    case resize_with_realloc:
      S(de_chain)(h);
    	ret = (struct S(header) *)realloc(h, n);
      ret->cs.mem_block_size = n;
      S(chain)(ret, state);
      break;
    case resize_with_malloc:
    	ret = (struct S(header) *)malloc(n);
    	memcpy(ret, h, h->cs.mem_block_size);
      ret->cs.mem_block_size = n;
      S(chain)(ret, state);
      break;
    case resize_with_identity:
      ret = h;
      break;
  }
  return ret;
}