#include "cee.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
using namespace cee;

void foo (char * x) {
  state::data * st = state::mk(10);
  str::data * s = str::mk(st, "logfile %s", x);
  printf("%p\n", s);
  printf("%s\n", (char *)s);
  
  // optional 
  // del(s);
  del(st);
  return;
}

int main () {
  /* test str */
  state::data * st = state::mk(10);
  foo((char *)"hello world");
  str::data * s, * s1, * s2;
  
  s = str::mk(st, "the number ten: %d", 10);
  printf("%s\n", (char *)s);
  
  s1 = str::mk(st, "the number ten point three: %.1f", 10.3);
  printf("%s\n", (char *)s1);
  
  s2 = str::mk(st, "%s, %s", s, s1);
  printf("%s\n", s2->_);
  
  /* test list */
  list::data *list = list::mk(st, 10);
  
  list::append(&list, s);
  list::append(&list, s1);
  list::append(&list, s2);
  
  printf("v.size %uz\n", list::size(list));
  int i;
  for (i = 0; i < list::size(list); i++)
    printf ("%d:%s\n", i, (char *)list->_[i]);
  
  // optional
  //del(list);
  
  /* heterogeneous list [ 10, 10.0, "10"] */
  enum T {
    I_T,
    F_T,
    S_T,
  };
  
  list = list::mk(st, 10);
  list::append(&list, tagged::mk(st, I_T, boxed::from_i32(st, 10)));
  list::append(&list, tagged::mk(st, F_T, boxed::from_float(st, 10.1)));
  list::append(&list, tagged::mk(st, S_T, str::mk(st, "10")));
  
  // optional
  //del(list);
  state::add_gc_root(st, list);
  
  /* test set */
  set::data * set1 = NULL;
  set1 = set::mk(st, (cmp_fun)strcmp);
  
  printf ("st: %p\n", set1);
  set::add(set1, str::mk(st, "a"));
  set::add(set1, str::mk(st, "aabc"));
  char * p = (char *)set::find(set1, (char *)"aabc");
  printf ("%s\n", p);
  
  set::remove(set1, str::mk(st, "aabc"));
  p = (char *)set::find(set1, (char *)"aabc");
  printf ("%s\n", p);
  
  list::data * svals = NULL;
  svals = set::values(set1);
  for (i = 0; i < list::size(svals); i++)
    printf ("%d %s\n", i, svals->_[i]);
  
  // optional
  // del(set);
  // del(svals);
  state::add_gc_root(st, set1);
  
  /* test map */
  map::data * mp = NULL;
  mp = map::mk(st, (cmp_fun)strcmp);
  
  map::add(mp, str::mk(st, "1"), boxed::from_i32(st, 10));
  map::add(mp, str::mk(st, "2"), boxed::from_i32(st, 20));
  map::add(mp, str::mk(st, "3"), boxed::from_i32(st, 30));
  
  boxed::data * t = (boxed::data *)map::find(mp, (char *)"1");
  printf ("found value %d\n", boxed::to_i32(t));
  
  list::data * keys = map::keys(mp);
  for (i = 0; i < list::size(keys); i++)
    printf ("[%d] key:%s\n", i, (char *)keys->_[i]);
  
  // optional
  //del(keys);
  //del(mp);
  state::add_gc_root(st, mp);
  
  /* test stack */
  stack::data * sp = stack::mk(st, 100);
  stack::push(sp, str::mk(st, "1"));
  stack::push(sp, str::mk(st, "2"));
  stack::push(sp, str::mk(st, "3"));
  printf ("%s\n", stack::top(sp, 0));
  
  // optional
  // del(sp);
  state::add_gc_root(st, sp);
  
  /* test diction */
  dict::data * dict = dict::mk(st, 1000);
  
  for (i = 0; i < 10; i++)
    dict::add(dict, str::mk(st, "%d", i)->_, str::mk(st, "value %d", i));

  str::data * key = str::mk(st, "9");
  printf ("%s\n", dict::find(dict, key->_));
  
  // optional
  // del(key);
  // del(dict);
  
  n_tuple::data * t5 = 
    n_tuple::mk(st, 5, str::mk(st, "1"), str::mk(st, "2"), str::mk(st, "3"), 
                str::mk(st, "4"), str::mk(st, "5"));
  
  for (i = 0; i < 5; i++)
    printf("%d, %s\n", i, t5->_[i]);
  
  printf("t5:%p\n", t5);
  state::add_gc_root(st, t5);
  printf("%uz\n", set::size(st->roots));
  
  list::data * roots = set::values(st->roots);
  for (i = 0; i < list::size(roots); i++) {
    printf ("%d:%p\n", i, roots->_[i]);
  }
  
  
  // optional
  state::gc(st);
  
  state::remove_gc_root(st, t5);
  printf("%zu\n", set::size(st->roots));
  state::gc(st);
  
  state::remove_gc_root(st, mp);
  printf("%zu\n", set::size(st->roots));
  state::gc(st);
  
  state::remove_gc_root(st, list);
  printf("%zu\n", set::size(st->roots));
  state::gc(st);
  
  state::remove_gc_root(st, sp);
  printf("%zu\n", set::size(st->roots));
  state::gc(st);
  
  state::remove_gc_root(st, set1);
  printf("%zu\n", set::size(st->roots));
  state::gc(st);
  
  // del(t5);
  // del(st);
  printf ("exit\n");
  return 0;
}