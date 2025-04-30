CC=g++
CFLAGS= -c -Wall -O3

all: compile run

compile: main.o graph.o dijkstra.o
	$(CC) build/main.o build/graph.o build/dijkstra.o -o build/Dijkstra

main.o:
	mkdir -p build
	$(CC) $(CFLAGS) source/main.cpp -o build/main.o
graph.o:
	$(CC) $(CFLAGS) source/graph.cpp -o build/graph.o
dijkstra.o:
	$(CC) $(CFLAGS) source/dijkstra.cpp -o build/dijkstra.o

run:
	echo 
	./build/Dijkstra

clean:
	rm -rf build/*.o