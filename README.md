# A minimalist C container library in C++ namespace

The goal is to improve C lover's productivity for "*high level*" programming 
by providing similar functionalities of C++ STL, but it does not intend to 
replicate C++ STL function by function.  This is reimplementation of [cee-std](https://github.com/cee-studio/cee-std) 
with C++'s namespace to organize symbols. The readability of the code is 
significantly improved. It does not intend to replicate C++ STL function by 
function.  It relies on C's memory layout to achieve interoperability with 
idiomatic C code without requiring any wrappers.



## It's optimized for the following use cases:
1. If you need simple C style containers for data oriented programming, 
   and have the full control of how memory are allocated and freed.
                                                                                
2. If you want to develop your own dynamic typed scripting languages but 
   you don't want to reinvent a runtime system. 

## How to use it ?

1. Download the two files;
```
  wget https://raw.githubusercontent.com/cee-studio/cee-std-ns/master/release/cee.h
  wget https://raw.githubusercontent.com/cee-studio/cee-std-ns/master/release/cee.cc
```
2. Add them to your source folder


## Usage Examples

**string**
```
  #include "cee.h"
 
  using namespace cee;

  str::data * s, * s1, * s2;
  
  s = str::mk("the number ten: %d", 10);
  printf("%s\n", (char *)s);
  
  s1 = str::mk("the number ten point three: %.1f", 10.3);
  printf("%s\n", (char *)e);
  
  s2 = str::mk("%s, %s", s, s1);
  printf("%s\n", (char *)s2);

  // delete strings
  del(s);
  del(s1);
  del(s2);
```

**array** (auto expandable)
```
  #include "cee.h"
 
  using namespace cee;

  array::data *v;
  
  v = array::mk(1);
  v = array::append(v, str::mk("1"));
  v = array::append(v, str::mk("2"));
  v = array::append(v, str::mk("3"));
  
  printf("v.size %u\n", array::size(v));
  for (int i = 0; i < array::size(v); i++) {
    printf ("%d:%s\n", i, (char *)v->_[i]);
  }

  // delete array
  del(v);
```

**set**
```
  #include "cee.h"

  using namespace cee;

  set::data * st = set::mk((cmp_fun)strcmp);
  printf ("st: %p\n", st);
  set::add(st, str::mk("a"));
  set::add(st, str::mk("aabc"));
  char * p = (char *)set::find(st, "aabc");
  printf ("%s\n", p);

  // delete set 
  del(st);
```

**map**
```
  #include "cee.h"

  using namespace cee;

  map::data * mp = map::mk((cmp_fun)strcmp);  
  map::add(mp, str::mk("1"), box::from_i32(10));
  map::add(mp, str::mk("2"), box::from_i32(20));
  map::add(mp, str::mk("3"), box::from_i32(30));
  
  void * t = map::find(mp, "1");
  printf ("found value %u\n", (uintptr_t)t);
  
  vect::data * keys = map::keys(mp);
  for (int i = 0; i < vect::count(keys); i++) {
    printf ("[%d] key:%s\n", i, (char *)keys->_[i]);
  }
 
  // delete map
  del(mp);
```

**stack**
```
  #include "cee.h"

  using namespace;

  stack::data * sp = stack::mk_e(dp_noop, 100);
  stack::push(sp, "1");
  stack::push(sp, "2");
  stack::push(sp, "3");
  printf ("%s\n", stack::top(sp, 0));

  // delete stack, optional
  del(stack);
```

**free any memory blocks of `cee::*::data`**

any memory blocks of a `cee::*::data` type can be freed with `cee::del` like the following:
```
   #include "cee.h"

   using namespace cee;

   str::data * s = ..;
   del(s);

   vect::data * v = ..;
   del(v);

   map::data * m = ..;
   del(m);

   stack::data * sp = ..;
   del(sp);
```

## How to test/develop it ?

### Using https://www.cee.studio cloud IDE

cee.studio is our primary development IDE as it can automatically detect and 
report all memory access violations, and memory leaks.

1. click [https://cee.studio/?bucket=orca&name=cee-std-ns](https://cee.studio/?bucket=orca&name=cee-std-ns)
2. clone to my account
3. click Start
4. run it Terminal




### Using your computer

You will need to install `valgrind` to debug memory access violations, and memory leaks.

```
git clone https://github.com/cee-studio/cee-std-ns.git
cd cee-std-ns
make
./a.out
valgrind ./a.out
```

## Rationale

[DESIGN](./DESIGN.md)


## Contribution is welcome

Please join our discord [https://discord.gg/nBUqrWf](https://discord.gg/nBUqrWf)
