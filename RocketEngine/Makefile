CC=g++

SRC=./src
EXAMPLES=./examples
DEPS=$(SRC)/ModelRocketEngine.h $(SRC)/EstesF15_4.h
CFLAGS=-fPIC -Wall -I $(SRC) -I . -I ../Jet/src

LIBS=../Jet/Jet
LFLAGS=-shared

OBJ=EstesF15_4.o ModelRocketEngine.o
OLIB=libModelRocketEngine.so


%.o: $(SRC)/%.cpp $(DEPS) Makefile
	$(CC) -c $< -o $@ $(CFLAGS)

library: $(OBJ)
	$(CC) -o $(OLIB) $(OBJ) $(LFLAGS) 

install: library $(OLIB) $(DEPS)
	install -m 755 -p $(OLIB) /usr/lib/
	install -m 644 -p $(DEPS) /usr/include/

uninstall:
	rm -f /usr/include/ModelRocketEngine.h
	rm -f /usr/include/EstesF15_4.h
	rm -f /usr/lib/libModelRocketEngine.so
	rm -f Takeoff*.*

clean:
	rm -f Takeoff
	rm -f Takeoff.o
	rm -f ModelRocketEngine.o
	rm -f EstesF15_4.o
	rm -f libModelRocketEngine.so

# Individual examples:

Takeoff.o: $(EXAMPLES)/Takeoff.cpp
	$(CC) -c $(EXAMPLES)/Takeoff.cpp -o $@ $(CFLAGS)

example: Takeoff.o library
	$(CC) Takeoff.o -o Takeoff -l ModelRocketEngine -l Jet -L ./ -L ../Jet
