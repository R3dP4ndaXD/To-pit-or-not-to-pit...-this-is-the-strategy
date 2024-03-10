CC = gcc
CFLAGS = -Wall -g

build: main

main: main.o operations.o functii.o
	$(CC) -o main $^ 

main.o: main.c functii.h
	$(CC) $(CFLAGS) -c $^

operations.o: operations.c
	$(CC) $(CFLAGS) -c $^

functii.o: functii.c functii.h
	$(CC) $(CFLAGS) -c $^

run: build
	./main

clean:
	rm -f *.o main