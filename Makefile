CC=gcc
CXX=g++-4.9
RM=rm -f
CPPFLAGS=-g -std=c++11 -pthread -I/sw/include/root 
LDFLAGS=-g
LDLIBS=

SRCS=main.cc
OBJS=$(subst .cc,.o,$(SRCS))

all: ciri

ciri: $(OBJS)
	$(CXX) $(LDFLAGS) -o ciri $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) *~ .depend

include .depend