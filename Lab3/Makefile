CC = g++
CFLAGS = -std=c++11
LINKFLAGS = -pedantic -Wall -fomit-frame-pointer -funroll-all-loops -O3
all:Lab3
Lab3:lab3.cpp place.o
	$(CC) $(CFLAGS) $(LINKFLAGS) lab3.cpp place.o -o Lab3
place.o:place.cpp
	$(CC) $(CFLAGS) $(LINKFLAGS) -c place.cpp
clean:
	rm -rf *.o Lab3