CC=g++

SRC=./src
EXAMPLES=./examples
INCS=$(SRC)/*.h
DEPS=$(INCS) $(SRC)/*
CFLAGS=-fPIC -Wall -I $(SRC)

LIBS=Servo Gimbal BMP180 BNO055 EDF Jet SimpleKalmanFilter Telemetry Serial gps GPS
LFLAGS=-shared

OBJ=Rocket.o rockhopper.o
OLIB=libRocket.so

%.o: $(SRC)/%.cpp $(DEPS) Makefile
	$(CC) -c $< -o $@ $(CFLAGS)

library: $(OBJ)
	$(CC) -o $(OLIB) $(OBJ) $(LFLAGS) -L /usr/lib/arm-linux-gnueabihf/ -L /usr/lib/x86_64-linux-gnu/ -lgps -lGPS -lGimbal -lServo -lGimbal -lBMP180 -lBNO055 -lEDF -lJet -lSimpleKalmanFilter -lTelemetry -lControl -lserial

install: library $(OLIB) $(INCS)
	install -m 755 -p $(OLIB) /usr/lib/
	install -m 644 -p $(INCS) /usr/include/

uninstall:
	rm -f /usr/include/Rocket.h /usr/include/rockhopper.h
	rm -f /usr/lib/$(OLIB)

clean:
	rm -f rockettome rocinantethehorse rockhoppertest
	rm -f *.o
	rm -f *.so

# Individual examples:

rockhoppertest.o: $(EXAMPLES)/rockhoppertest.cpp library
	$(CC) -c $(EXAMPLES)/rockhoppertest.cpp -o $@ $(CFLAGS)

examples: rockhoppertest.o libRocket.so
	$(CC) rockhoppertest.o -o rockhopper -L /usr/lib/arm-linux-gnueabihf/ -L /usr/lib/x86_64-linux-gnu/ -lgps -lGPS -lGimbal -lServo -lGimbal -lBMP180 -lBNO055 -lEDF -lJet -lSimpleKalmanFilter -lTelemetry -lRocket -lControl -lserial
	
