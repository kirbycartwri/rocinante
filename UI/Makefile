CC=g++

SRC=./src
EXAMPLES=./examples
DEPS=$(SRC)/*
INCS=$(SRC)/*.h
CFLAGS=-fPIC -Wall -I $(SRC)

LIBS=curses menu BMP180 BNO055 Control EDF Jet Rocket Servo SimpleKalmanFilter Telemetry UI serial pthread gps GPS
LFLAGS=-shared

OBJ=stdin_ui.o
OLIB=libUI.so

%.o: $(SRC)/%.cpp $(DEPS) Makefile
	$(CC) -c $< -o $@ $(CFLAGS)

library: $(OBJ)
	$(CC) -o $(OLIB) $(OBJ) $(LFLAGS) -L /usr/lib/arm-linux-gnueabihf/ -L /usr/lib/x86_64-linux-gnu/ -lpthread -lcurses -lmenu -lBMP180 -lBNO055 -lControl -lEDF -lJet -lRocket -lServo -lSimpleKalmanFilter -lTelemetry -lserial -lgps -lGPS

install: library $(OLIB) $(DEPS)
	install -m 755 -p $(OLIB) /usr/lib/
	install -m 644 -p $(INCS) /usr/include/

uninstall:
	rm -f /usr/include/stdin_ui.h
	rm -f /usr/lib/libUI.so

clean:
	rm -f stdin_ui
	rm -f *.o
	rm -f *.so

# Individual examples: RocketUI_n.cpp  RocketUI_p.cpp  RocketUI_s.cpp  test_ncurses.cpp
# Compile ncurses with "gcc test_ncurses.cpp -lmenu -lncurses"

RocketUI_s.o: $(EXAMPLES)/RocketUI_s.cpp library
	$(CC) -c $(EXAMPLES)/RocketUI_s.cpp -o $@ $(CFLAGS)


examples: RocketUI_s.o libUI.so Makefile
	$(CC) RocketUI_s.o -o RocketUI_s -L /usr/lib/arm-linux-gnueabihf/ -L /usr/lib/x86_64-linux-gnu/ -lpthread -lcurses -lmenu -lBMP180 -lBNO055 -lControl -lEDF -lJet -lRocket -lServo -lSimpleKalmanFilter -lTelemetry -lUI -lserial -lgps -lGPS

	
