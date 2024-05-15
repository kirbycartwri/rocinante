CC=g++

SRC=./src
EXAMPLES=./examples
DEPS=$(SRC)/BMP180.h
CFLAGS=-fPIC -Wall -I $(SRC)

LIBS=SimpleKalmanFilter
LFLAGS=-shared

OBJ=BMP180.o
OLIB=libBMP180.so


%.o: $(SRC)/%.cpp $(DEPS) Makefile
	$(CC) -c $< -o $@ $(CFLAGS)

library: $(OBJ)
	$(CC) -o $(OLIB) $< $(LFLAGS) -l $(LIBS)

install: library $(OLIB) $(DEPS)
	install -m 755 -p $(OLIB) /usr/lib/
	install -m 644 -p $(DEPS) /usr/include/

uninstall:
	rm -f /usr/include/BMP180.h
	rm -f /usr/lib/libBMP180.so

clean:
	rm -f AltitudeKalmanFilterExample
	rm -f *.o
	rm -f *.so

# Individual examples:

AltitudeKalmanFilterExample.o: $(EXAMPLES)/AltitudeKalmanFilterExample.cpp $(DEPS) $(OLIB)
	$(CC) -c $(EXAMPLES)/AltitudeKalmanFilterExample.cpp -o $@ $(CFLAGS)

example: library AltitudeKalmanFilterExample.o libBMP180.so
	$(CC) AltitudeKalmanFilterExample.o -o AltitudeKalmanFilterExample -l BMP180 -l $(LIBS)
