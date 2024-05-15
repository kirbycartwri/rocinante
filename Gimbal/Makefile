CC=g++

SRC=./src
EXAMPLES=./examples
DEPS=$(SRC)/* ../Servo/src/*
INCS=$(SRC)/*.h
CFLAGS=-fPIC -Wall -I $(SRC)

LIBS=Servo Jet
LFLAGS=-shared

OBJ=K_9_TVC_Gimbal_Generation_2.o Gimbal.o 

OLIB=libGimbal.so


%.o: $(SRC)/%.cpp $(DEPS) Makefile
	$(CC) -c $< -o $@ $(CFLAGS)

library: $(OBJ) $(DEPS)
	$(CC) -o $(OLIB) $(OBJ) $(LFLAGS) -l Servo -l Jet

install: library $(OLIB) $(DEPS)
	install -m 755 -p $(OLIB) /usr/lib/
	install -m 644 -p $(INCS) /usr/include/

uninstall:
	rm -f /usr/include/Gimbal.h
	rm -f /usr/include/K_9_TVC_Gimbal_Generation_2.h
	rm -f /usr/lib/libGimbal.so

clean:
	rm -f SweepPitchAndYaw GimbalControl
	rm -f *.o
	rm -f *.so

# Individual examples:

GimbalControl.o: $(EXAMPLES)/GimbalControl.cpp library
	$(CC) -c $(EXAMPLES)/GimbalControl.cpp -o $@ $(CFLAGS)

SweepPitchAndYaw.o: $(EXAMPLES)/SweepPitchAndYaw.cpp library
	$(CC) -c $(EXAMPLES)/SweepPitchAndYaw.cpp -o $@ $(CFLAGS)

examples: SweepPitchAndYaw.o $(OLIB) GimbalControl.o 
	$(CC) SweepPitchAndYaw.o -o SweepPitchAndYaw -l Gimbal -l Servo -l Jet
	$(CC) GimbalControl.o -o GimbalControl -l Gimbal -l Servo -l Jet
	
