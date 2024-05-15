CC=g++

SRC=./src
EXAMPLES=./examples
DEPS=$(SRC)/Telemetry.h $(SRC)/RYLR406.h
CFLAGS=-fPIC -Wall -I $(SRC)

LIBS=
LFLAGS=-shared

OBJ=RYLR406.o Telemetry.o
OLIB=libTelemetry.so


%.o: $(SRC)/%.cpp $(DEPS) Makefile
	$(CC) -c $< -o $@ $(CFLAGS)

library: $(OBJ)
	$(CC) -o $(OLIB) $(OBJ) $(LFLAGS) 

install: library $(OLIB) $(DEPS)
	install -m 755 -p $(OLIB) /usr/lib/
	install -m 644 -p $(DEPS) /usr/include/

uninstall:
	rm -f /usr/include/Telemetry.h
	rm -f /usr/include/RockHopper.h
	rm -f /usr/lib/Telemetry.so
	rm -f stdout*.*

clean:
	rm -f stdout
	rm -f stdout.o
	rm -f Telemetry.o
	rm -f RYLR406.o
	rm -f libTelemetry.so

# Individual examples:

stdout.o: $(EXAMPLES)/stdout.cpp
	$(CC) -c $(EXAMPLES)/stdout.cpp -o $@ $(CFLAGS)

example: stdout.o library
	$(CC) stdout.o -l Telemetry -o stdout
