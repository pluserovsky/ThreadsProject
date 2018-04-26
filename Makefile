all: main
main: p1.cpp
	g++ -std=c++11 -pthread  p1.cpp -o race Car.cpp Car.h Pitstop.cpp Pitstop.h -lncurses
test: all
	./race
clean:
	rm -rf *.o race
