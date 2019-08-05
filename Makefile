.PHONY:	all clean clobber install

all:	unittest

install: all

OBJS	= main.o

LDFLAGS = -L$(PREFIX)/lib 

unittest: unittest.o
	$(CXX) -o unittest unittest.o $(LDFLAGS)

clean:	
	rm -f *.o *.x1o a.out core core.*

clobber: clean
	@rm -f .errs.t unittest

include Makefile.incl
PREFIX	= $(HOME)/local

# End Makefile
