#include "cee.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
using namespace cee;

void foo (char * x) {
  state::data * st = state::mk();
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
  state::data * st = state::mk();
  foo((char *)"hello world");
  str::data * s, * s1, * s2;
  
  s = str::mk(st, "the number ten: %d", 10);
  printf("%s\n", (char *)s);
  
  s1 = str::mk(st, "the number ten point three: %.1f", 10.3);
  printf("%s\n", (char *)s1);
  
  s2 = str::mk(st, "%s, %s", s, s1);
  printf("%s\n", s2->_);
  
  /* test list */
  list::data *v = list::mk(st, 10);
  
  list::append(&v, s);
  list::append(&v, s1);
  list::append(&v, s2);
  
  printf("v.size %uz\n", list::size(v));
  int i;
  for (i = 0; i < list::size(v); i++)
    printf ("%d:%s\n", i, (char *)v->_[i]);
  
  // optional
  // del(v);
  
  /* heterogeneous list [ 10, 10.0, "10"] */
  enum T {
    I_T,
    F_T,
    S_T,
  };
  
  v = list::mk(st, 10);
  list::append(&v, tagged::mk(st, I_T, boxed::from_i32(st, 10)));
  list::append(&v, tagged::mk(st, F_T, boxed::from_float(st, 10.1)));
  list::append(&v, tagged::mk(st, S_T, str::mk(st, "10")));
  
  // optional
  // del(v);
  
  /* test set */
  set::data * set = NULL;
  set = set::mk(st, (cmp_fun)strcmp);
  
  printf ("st: %p\n", set);
  set::add(set, str::mk(st, "a"));
  set::add(set, str::mk(st, "aabc"));
  char * p = (char *)set::find(set, (char *)"aabc");
  printf ("%s\n", p);
  
  list::data * svals = NULL;
  svals = set::values(set);
  for (i = 0; i < list::size(svals); i++)
    printf ("%d %s\n", i, svals->_[i]);
  
  // optional
  // del(set);
  // del(svals);
  
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
  // del(keys);
  // del(mp);
  
  /* test stack */
  stack::data * sp = stack::mk(st, 100);
  stack::push(sp, str::mk(st, "1"));
  stack::push(sp, str::mk(st, "2"));
  stack::push(sp, str::mk(st, "3"));
  printf ("%s\n", stack::top(sp, 0));
  
  // optional
  // del(sp);
  
  /* test diction */
  dict::data * dict = dict::mk(st, 1000);
  
  for (i = 0; i < 1000; i++)
    dict::add(dict, str::mk(st, "%d", i)->_, str::mk(st, "value %d", i));

  str::data * key = str::mk(st, "999");
  printf ("%s\n", dict::find(dict, key->_));
  
  // optional
  // del(key);
  // del(dict);
  
  n_tuple::data * t5 = 
    n_tuple::mk(st, 5, str::mk(st, "1"), 
                str::mk(st, "2"), str::mk(st, "3"), 
                str::mk(st, "4"), str::mk(st, "5"));
  
  for (i = 0; i < 5; i++)
    printf("%d, %s\n", i, t5->_[i]);
  
  // optional
  // del(t5);
  state::gc(st);
  //del(st);
  return 0;
}