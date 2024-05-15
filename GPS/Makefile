CC=g++

SRC=./src
EXAMPLES=./examples
DEPS=$(SRC)/GPS.h
CFLAGS=-fPIC -Wall -I $(SRC)

LIBS=
LFLAGS=-shared

OBJ=GPS.o
OLIB=libGPS.so


%.o: $(SRC)/%.cpp $(DEPS) Makefile
	$(CC) -c $< -o $@ $(CFLAGS)

library: $(OBJ) $(DEPS)
	$(CC) -o $(OLIB) $< $(LIBS) $(LFLAGS) -l GPS -l gps

install: library $(OLIB) $(DEPS)
	install -m 755 -p $(OLIB) /usr/lib/
	install -m 644 -p $(DEPS) /usr/include/

uninstall:
	rm -f /usr/include/GPS.h
	rm -f /usr/lib/$(OLIB)

clean:
	rm -f location
	rm -f *.o
	rm -f *.so

# Individual examples:

location.o: $(EXAMPLES)/location.cpp $(DEPS) $(OLIB)
	$(CC) -c $(EXAMPLES)/location.cpp -o $@ $(CFLAGS)

example: $(OLIB) location.o
	$(CC) location.o -o location -l GPS -l gps
