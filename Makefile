.PHONY:	all clean clobber install

all:	unittest

install: all

OBJS	= main.o

LDFLAGS = -L$(PREFIX)/lib 

unittest: unittest.o unittest.dat
	$(CXX) -o unittest unittest.o $(LDFLAGS)
	./unittest <unittest.dat

clean:	
	rm -f *.o *.x1o a.out core core.*

clobber: clean
	@rm -f .errs.t unittest

include Makefile.incl
PREFIX	= $(HOME)/local

unittest.o: chartree.hpp

# End Makefile
