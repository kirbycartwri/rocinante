CC=g++

SRC=./src
EXAMPLES=./examples
DEPS=$(SRC)/*
INCS=$(SRC)/*.h
FILES=$(SRC)/*.cpp
CFLAGS=-fPIC -Wall -I $(SRC)

LIBS=Servo Jet BNO055
LFLAGS=-shared

OBJ=RockHopperControl.o Control.o
OLIB=libControl.so


%.o: $(SRC)/%.cpp $(DEPS) Makefile $(FILES)
	$(CC) -c $< -o $@ $(CFLAGS)

library: $(OBJ)
	$(CC) -o $(OLIB) $(OBJ) $(LFLAGS) -l Servo -l Jet -l BNO055

install: library $(OLIB) $(DEPS)
	install -m 755 -p $(OLIB) /usr/lib/
	install -m 644 -p $(INCS) /usr/include/

uninstall:
	rm -f /usr/include/Control.h
	rm -f /usr/include/RockHopperControl.h
	rm -f /usr/lib/$(OLIB)
	rm -f Simulate*.*

clean:
	rm -f SimulateControl
	rm -f *.o
	rm -f *.so

# Individual examples:

SimulateControl.o: $(EXAMPLES)/SimulateControl.cpp
	$(CC) -c $(EXAMPLES)/SimulateControl.cpp $(CFLAGS)

example: SimulateControl.o library
	$(CC) SimulateControl.o -o SimulateControl -l Control -l Servo -l Jet -l BNO055 
