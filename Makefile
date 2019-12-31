all: tinyWebServer

tinyWebServer: main.o implementA.o implementB.o
	gcc -W -Wall -lpthread -o tinyWebServer main.o implementA.o implementB.o
.PHONY: clean
clean:
	rm  *.o tinyWebServer
