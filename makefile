CFLAGS = `sdl2-config --cflags` -Wall -Wextra -pedantic -Wfloat-equal -ansi -O2
VALGRIND = --error-exitcode=1 --quiet --leak-check=full
INCS = header.h
TARGET = parser
SOURCES =  $(TARGET).c neillsdl2.c interpreter.c test.c
LIBS =  `sdl2-config --libs` -lm
CC = gcc


all: $(TARGET)

$(TARGET): $(SOURCES) $(INCS)
	$(CC) $(SOURCES) -o $(TARGET) $(CFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET) 
