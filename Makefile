all: tinyWebServer

tinyWebServer: main.o implementA.o implementB.o
	gcc -W -Wall -o tinyWebServer main.o implementA.o implementB.o -lpthread
.PHONY: clean
clean:
	rm  *.o tinyWebServer
