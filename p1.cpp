#include "Car.h"

using namespace std;

int amountOfCars = 2;
int amountOfLaps = 2;
bool finish = false;

void run(Car car)
{
	for (int i = 0; i < amountOfLaps; i++)
	{
		if (!finish)
		{
			move(car.getID(), i + 8);
			printw("%c", car.driving());

			if (car.getLaps() == amountOfLaps)
			{
				printw(" Car %i won!", car.getID());
				finish = true;
				refresh();
				break;
			}
			usleep(rand() % 1000000);
			move(car.getID(), i + 40);
			printw("%c", car.cooling());
			usleep(rand() % 1000000);
			refresh();
		}
	}
}

bool checkParameters(int argc, char **argv){
	if (argc != 3)
	{
		printf("run parameters: [amountOfCars] [amountOfLaps]\n");
		endwin();
		return 0;
	}
	amountOfCars = atoi(argv[1]);
	amountOfLaps = atoi(argv[2]);

	if (amountOfCars > 15)
	{
		printf("Max amount of cars is 15!\n");
		endwin();
		return 0;
	}

	if (amountOfLaps > 20)
	{
		printf("Max amount of laps is 20!\n");
		endwin();
		return 0;
	}
	return 1;
}

int main(int argc, char **argv)
{
	initscr();
	srand(time(NULL));
	if(!checkParameters(argc,argv)) return 0;

	thread t[amountOfCars];
	Car *cars = new Car[amountOfCars];
	refresh();

	for (int i = 0; i < amountOfCars; i++)
	{
		move(i, 0);
		printw("Car %i:", i);
		move(i, 30);
		printw("stop's:", i);
		Car car(i);
		cars[i] = car;
		t[i] = thread(run, car);
	}

	for (int i = 0; i < amountOfCars; i++)
	{
		t[i].join();
	}

	getch();
	endwin();

	return 0;
}
