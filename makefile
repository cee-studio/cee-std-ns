CEE_SRC=musl-search.h musl-hsearch.c musl-insque.c musl-lsearch.c musl-tsearch.c \
  cee-common.cpp boxed.cpp str.cpp dict.cpp map.cpp set.cpp stack.cpp \
  tuple.cpp triple.cpp quadruple.cpp list.cpp tagged.cpp singleton.cpp \
  closure.cpp block.cpp n_tuple.cpp env.cpp state.cpp 

CXXFLAGS= -fno-rtti -fno-exceptions -g

HEADERS=stddef.h stdio.h string.h stdlib.h stdarg.h assert.h errno.h

define cee_amalgamation
	@echo "#define CEE_AMALGAMATION" > tmp.cpp
	@for ii in $(CEE_SRC); do echo '#include "'$$ii'"' >> tmp.cpp; done
	@echo "#define CEE_ONE" >> $(1)
	@for ii in $(HEADERS); do echo '#include <'$$ii'>' >> $(1); done
	@cat cee.hpp >> $(1)
	@echo " " >> $(1)
	@cat cee-internal.h >> $(1)
	$(CXX) -E -CC $(2) -nostdinc tmp.cpp >> $(1)
endef

.PHONY: release clean distclean

all: tester

cee-one.cpp: $(CEE_SRC)
	$(call cee_amalgamation, cee-one.cpp)

cee-one.o: cee-one.cpp
	$(CXX) -c $(CXXFLAGS) cee-one.cpp


release:
	$(call cee_amalgamation, cee.cpp,-P)
	@mkdir -p release
	@mv cee.cpp  release
	@cp cee.hpp  release

tester: cee-one.o
	$(CXX) -static $(CXXFLAGS) tester.cpp cee-one.o

clean:
	rm -f cee.cpp tmp.cpp cee-one.* a.out