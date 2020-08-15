#include "cee.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
using namespace cee;

void foo (char * x) {
  str::data * s = str::mk("logfile %s", x);
  printf("%p\n", s);
  printf("%s\n", (char *)s);
  del(s);
  return;
}

int main () {
  /* test str */
  foo((char *)"hello world");
  str::data * s, * s1, * s2;
  
  s = str::mk("the number ten: %d", 10);
  printf("%s\n", (char *)s);
  
  s1 = str::mk("the number ten point three: %.1f", 10.3);
  printf("%s\n", (char *)s1);
  
  s2 = str::mk("%s, %s", s, s1);
  printf("%s\n", s2->_);
  
  /* test vector */
  vect::data *v, *v1, *v2;
  
  v = vect::mk(1);
  use_realloc(v);
  
  v = vect::append(v, s);
  v = vect::append(v, s1);
  v = vect::append(v, s2);
  
  printf("v.count %u\n", vect::size(v));
  int i;
  for (i = 0; i < vect::size(v); i++)
    printf ("%d:%s\n", i, (char *)v->_[i]);
  
  del(v);
  
  /* a dynamic typed array */
  v = vect::mk(1);
  use_realloc(v);
  
  v = vect::append(v, tagged::mk(1, boxed::from_i32(10)));
  v = vect::append(v, tagged::mk(2, boxed::from_float(10.1)));
  v = vect::append(v, tagged::mk(3, str::mk("10")));
  del(v);
  
  /* test set */
  set::data * st = NULL;
  st = set::mk((cmp_fun)strcmp);
  
  printf ("st: %p\n", st);
  set::add(st, str::mk("a"));
  set::add(st, str::mk("aabc"));
  char * p = (char *)set::find(st, (char *)"aabc");
  printf ("%s\n", p);
  
  vect::data * svals = NULL;
  svals = set::values(st);
  for (i = 0; i < vect::size(svals); i++)
    printf ("%d %s\n", i, svals->_[i]);
  
  del(st);
  del(svals);
  
  /* test map */
  map::data * mp = NULL;
  mp = map::mk((cmp_fun)strcmp);
  
  map::add(mp, str::mk("1"), boxed::from_i32(10));
  map::add(mp, str::mk("2"), boxed::from_i32(20));
  map::add(mp, str::mk("3"), boxed::from_i32(30));
  
  boxed::data * t = (boxed::data *)map::find(mp, (char *)"1");
  printf ("found value %d\n", boxed::to_i32(t));
  
  vect::data * keys = map::keys(mp);
  for (i = 0; i < vect::size(keys); i++)
    printf ("[%d] key:%s\n", i, (char *)keys->_[i]);
  
  del(keys);
  del(mp);
  
  /* test stack */
  stack::data * sp = stack::mk(100);
  stack::push(sp, str::mk("1"));
  stack::push(sp, str::mk("2"));
  stack::push(sp, str::mk("3"));
  printf ("%s\n", stack::top(sp, 0));
  del(sp);
  
  /* test diction */
  dict::data * dict = dict::mk(1000);
  
  for (i = 0; i < 1000; i++)
    dict::add(dict, str::mk("%d", i)->_, str::mk("value %d", i));

  str::data * key = str::mk("999");
  printf ("%s\n", dict::find(dict, key->_));
  del(key);
  del(dict);
  
  n_tuple::data * t5 = 
    n_tuple::mk(5, str::mk("1"), str::mk("2"), str::mk("3"), str::mk("4"), str::mk("5"));
  
  for (i = 0; i < 5; i++)
    printf("%d, %s\n", i, t5->_[i]);
  
  del(t5);
  return 0;
}