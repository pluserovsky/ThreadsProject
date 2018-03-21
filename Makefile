all: main
main: p1.cpp
	g++ -pthread p1.cpp -o p1 -lncurses
test: all
	./p1
clean:
	rm -rf *.o p1
