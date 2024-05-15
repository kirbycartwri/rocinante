CC=g++

SRC=./src
EXAMPLES=./examples
DEPS=$(SRC)/*
INCS=$(SRC)/*.h
CFLAGS=-fPIC -Wall -I $(SRC)

LIBS=
LFLAGS=-shared

OBJ=Jet.o
OLIB=libJet.so


%.o: $(SRC)/%.cpp $(DEPS) Makefile
	$(CC) -c $< -o $@ $(CFLAGS)

library: $(OBJ)
	$(CC) -o $(OLIB) $< $(LIBS) $(LFLAGS)

install: library $(OLIB) $(DEPS)
	install -m 755 -p $(OLIB) /usr/lib/
	install -m 644 -p $(INCS) /usr/include/

uninstall:
	rm -f /usr/include/Jet.h
	rm -f /usr/lib/$(OLIB)

clean:
	rm -f *.o
	rm -f *.so

# No examples
