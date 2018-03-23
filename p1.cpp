#include "Car.h"

using namespace std;

int amountOfCars = 2;
int amountOfLaps = 10;

void run(Car car)
{
	move(car.getID(),0);
	printw("Car %i:", car.getID());
	for(int i=0; i<amountOfLaps;i++)
	{
		move(car.getID(),i+7);
		printw("%c", car.driving());
		if(car.getLaps() == 5) {
			printw("Car %i won!", car.getID());
		}
		usleep(rand() % 500000);
		//printw("%c", car.cooling());
		usleep(rand() % 500000);
		refresh();
	}
}

int main(int argc, char **argv)
{
	initscr();
	srand(time(NULL));
	thread t[amountOfCars];
	Car *cars = new Car[amountOfCars];
	refresh();
	for(int i=0; i<amountOfCars;i++)
	{
		Car car(i);
		cars[i] = car;
		t[i] = thread(run,car);
	}
	
	for(int i=0; i<amountOfCars;i++)
	{
		t[i].join();
	}
	for(int i=0; i<amountOfCars;i++)
	{
		t[i].~thread();
	}
	getch();
	endwin();
	
	return 0;
}
