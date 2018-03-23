all: main
main: p1.cpp
	g++ -std=c++11 -pthread  p1.cpp -o p1 Car.cpp Car.h -lncurses
test: all
	./p1
clean:
	rm -rf *.o p1
