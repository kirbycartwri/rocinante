CC=g++

SRC=./src
EXAMPLES=./examples
DEPS=$(SRC)/BNO055.h
CFLAGS=-fPIC -Wall -I $(SRC)

LIBS=
LFLAGS=-shared

OBJ=BNO055.o
OLIB=libBNO055.so


%.o: $(SRC)/%.cpp $(DEPS) Makefile
	$(CC) -c $< -o $@ $(CFLAGS)

library: $(OBJ) $(DEPS)
	$(CC) -o $(OLIB) $< $(LIBS) $(LFLAGS)

install: library $(OLIB) $(DEPS)
	install -m 755 -p $(OLIB) /usr/lib/
	install -m 644 -p $(DEPS) /usr/include/

uninstall:
	rm -f /usr/include/BNO055.h
	rm -f /usr/lib/libBNO055.so

clean:
	rm -f bunny
	rm -f *.o
	rm -f *.so

# Individual examples:

bunny.o: $(EXAMPLES)/bunny.cpp $(DEPS) $(OLIB)
	$(CC) -c $(EXAMPLES)/bunny.cpp -o $@ $(CFLAGS)

example: $(OLIB) bunny.o
	$(CC) bunny.o -o bunny -l BNO055
