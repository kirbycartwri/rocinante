CC=g++

SRC=./src
EXAMPLES=./examples
DEPS=$(SRC)/*
INCS=$(SRC)/*.h
CFLAGS=-fPIC -Wall -I $(SRC) -I ../Servo/src -I ../Jet/src

LIBS=Servo Jet
LFLAGS=-shared

OBJ=DoBoFo70Pro12.o EDF.o
OLIB=libEDF.so

%.o: $(SRC)/%.cpp $(DEPS) Makefile
	$(CC) -c $< -o $@ $(CFLAGS)

library: $(OBJ)
	$(CC) -o $(OLIB) $(OBJ) $(LFLAGS) -l Servo -l Jet

install: library $(OLIB) $(DEPS)
	install -m 755 -p $(OLIB) /usr/lib/
	install -m 644 -p $(INCS) /usr/include/

uninstall:
	rm -f /usr/include/EDF.h
	rm -f /usr/include/DoBoFo70Pro12.h
	rm -f /usr/lib/$(OLIB)
	rm -f FanSweep*.* FanControl*.*

clean:
	rm -f FanSweep FanControl
	rm -f *.o
	rm -f *.so

# Individual examples:

FanControl.o: $(EXAMPLES)/FanControl.cpp
	$(CC) -c $(EXAMPLES)/FanControl.cpp -o $@ $(CFLAGS)

FanSweep.o: $(EXAMPLES)/FanSweep.cpp
	$(CC) -c $(EXAMPLES)/FanSweep.cpp -o $@ $(CFLAGS)

examples: FanSweep.o library FanControl.o
	$(CC) FanSweep.o -o FanSweep -l EDF -l Servo -l Jet
	$(CC) FanControl.o -o FanControl -l EDF -l Servo -l Jet
	
