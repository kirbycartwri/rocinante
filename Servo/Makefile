CC=g++

SRC=./src
EXAMPLES=./examples
DEPS=$(SRC)/*
INCS=$(SRC)/*.h
CFLAGS=-fPIC -Wall -I $(SRC)

LIBS=
LFLAGS=-shared

OBJ=Servo.o DorheaMG90S.o
OLIB=libServo.so


%.o: $(SRC)/%.cpp $(DEPS) Makefile
	$(CC) -c $< -o $@ $(CFLAGS)

library: $(OBJ)
	$(CC) -o $(OLIB) $(OBJ) $(LFLAGS) 

install: library $(OLIB) $(DEPS)
	install -m 755 -p $(OLIB) /usr/lib/
	install -m 644 -p $(INCS) /usr/include/

uninstall:
	rm -f /usr/include/Servo.h
	rm -f /usr/lib/libServo.so

clean:
	rm -f Sweep
	rm -f *.o
	rm -f *.so

# Individual examples:

Sweep.o: $(EXAMPLES)/Sweep.cpp 
	$(CC) -c $(EXAMPLES)/Sweep.cpp -o $@ $(CFLAGS)

ServoControl.o: $(EXAMPLES)/ServoControl.cpp 
	$(CC) -c $(EXAMPLES)/ServoControl.cpp -o $@ $(CFLAGS)

examples: Sweep.o ServoControl.o libServo.so
	$(CC) Sweep.o -o Sweep -lServo
	$(CC) ServoControl.o -o ServoControl -lServo
