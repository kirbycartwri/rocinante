CC=g++

SRC=./src
EXAMPLES=./examples
DEPS=$(SRC)/*
INCS=$(SRC)/*.h
CFLAGS=-fPIC -Wall -I $(SRC)

LIBS=curses menu pthread
LFLAGS=-shared

OBJ=CTextForm.o  CTextMenu.o  CTextWindow.o
OLIB=libNcursesCpp.so

%.o: $(SRC)/%.cpp $(DEPS) Makefile
	$(CC) -c $< -o $@ $(CFLAGS)

library: $(OBJ)
	$(CC) -o $(OLIB) $(OBJ) $(LFLAGS) -l pthread -l curses -l menu

install: library $(OLIB) $(DEPS)
	install -m 755 -p $(OLIB) /usr/lib/
	install -m 644 -p $(INCS) /usr/include/

uninstall:
	rm -f /usr/include/CTextForm.h
	rm -f /usr/include/CTextMenu.h	
	rm -f /usr/include/CTextWindow.h	
	rm -f /usr/lib/$(OLIB)

clean:
	rm -f NCTest
	rm -f *.o
	rm -f *.so

NCurses_Test.o: $(EXAMPLES)/NCurses_Test.cpp library
	$(CC) -c $(EXAMPLES)/NCurses_Test.cpp -o $@ $(CFLAGS)


examples: NCurses_Test.o $(OLIB) Makefile
	$(CC) NCurses_Test.o -o NCTest -l pthread -l curses -l menu -l NcursesCpp

	
