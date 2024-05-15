CC=g++

SRC=./src
EXAMPLES=./examples
DEPS=$(SRC)/*
INCS=$(SRC)/*.h
CFLAGS=-fPIC -Wall -I $(SRC)

LIBS=serial
LFLAGS=-shared

OBJ=RYLR406.o Telemetry.o
OLIB=libTelemetry.so


%.o: $(SRC)/%.cpp $(DEPS) Makefile
	$(CC) -c $< -o $@ $(CFLAGS)

library: $(OBJ)
	$(CC) -o $(OLIB) $(OBJ) $(LFLAGS) -L /usr/lib/x86_64-linux-gnu/ -L /usr/lib/arm-linux-gnueabihf/ -l $(LIBS)

install: library $(OLIB) $(DEPS)
	install -m 755 -p $(OLIB) /usr/lib/
	install -m 644 -p $(INCS) /usr/include/

uninstall:
	rm -f /usr/include/Telemetry.h
	rm -f /usr/include/RockHopper.h
	rm -f /usr/lib/$(OLIB)
	rm -f stdout*.*

clean:
	rm -f stdout
	rm -f *.o
	rm -f *.so

# Individual examples:

stdout.o: $(EXAMPLES)/stdout.cpp $(DEPS) $(OLIB)
	$(CC) -c $(EXAMPLES)/stdout.cpp -o $@ $(CFLAGS)

example: stdout.o
	$(CC) stdout.o -l Telemetry -o stdout -L /usr/lib/x86_64-linux-gnu/ -l $(LIBS)
