all: tinyWebServer

tinyWebServer: main.o implementA.o implementB.o
	gcc -W -Wall -g -o tinyWebServer main.o implementA.o implementB.o -lpthread
main.o: main.cpp
	gcc -g -c main.cpp -o main.o
implementA.o: implementA.cpp
	gcc -g -c implementA.cpp -o implementA.o
implementB.o: implementB.cpp
	gcc -g -c implementB.cpp -o implementB.o
.PHONY: clean
clean:
	rm  *.o tinyWebServer
