CC=g++

SRC=./src
EXAMPLES=./examples
DEPS=../Servo/src/Servo.h
CFLAGS=-fPIC -Wall -I $(SRC)

LIBS=Servo
LFLAGS=-shared

OBJ=Servo.o
OLIB=libServo.so


%.o: $(SRC)/%.cpp $(DEPS) Makefile
	$(CC) -c $< -o $@ $(CFLAGS)

library: $(OBJ)
	$(CC) -o $(OLIB) $< $(LIBS) $(LFLAGS)

install: library $(OLIB) $(DEPS)
	install -m 755 -p $(OLIB) /usr/lib/
	install -m 644 -p $(DEPS) /usr/include/

uninstall:
	rm -f /usr/include/Servo.h
	rm -f /usr/lib/libServo.so

clean:
	rm -f Sweep
	rm -f Sweep.o
	rm -f Servo.o
	rm -f libServo.so

# Individual examples:

Sweep.o: $(EXAMPLES)/Sweep.cpp
	$(CC) -c $(EXAMPLES)/Sweep.cpp -o $@ $(CFLAGS)

example: Sweep.o libServo.so
	$(CC) Sweep.o -o Sweep -lServo -L ./
