#include "Car.h"

using namespace std;

static const int amountOfCars = 2;
static const int amountOfLaps = 10;
bool finish = false;

thread t[amountOfCars];

void run(Car car)
{
	for(int i=0; i<amountOfLaps;i++)
	{
		if(!finish){
			move(car.getID(),i+7);
			printw("%c", car.driving());
			
			usleep(rand() % 1000000);
			move(car.getID(),i+40);
			printw("%c", car.cooling());
			usleep(rand() % 1000000);

			if(car.getLaps() == amountOfLaps) {
				printw(" Car %i won!", car.getID());
				finish = true;
			}

			refresh();
		}
	}
}

int main(int argc, char **argv)
{
	initscr();
	srand(time(NULL));
	
	Car *cars = new Car[amountOfCars];
	refresh();
	for(int i=0; i<amountOfCars;i++)
	{
		move(i,0);
		printw("Car %i:", i);
		move(i,30);
		printw("stop's:", i);
		Car car(i);
		cars[i] = car;
		t[i] = thread(run,car);
	}
	
	for(int i=0; i<amountOfCars;i++)
	{
		t[i].join();
	}

	getch();
	endwin();
	
	return 0;
}
