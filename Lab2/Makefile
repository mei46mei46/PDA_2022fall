CC = g++
CFLAGS = -std=c++11
all:Lab2
Lab2:lab2.cpp b_star.o
	$(CC) $(CFLAGS) lab2.cpp b_star.o -o Lab2
b_star.o:b_star.cpp
	$(CC) $(CFLAGS) -c b_star.cpp
clean:
	rm -rf *.o Lab2