#include "Car.h"

using namespace std;

int amountOfCars = 2;
int amountOfLaps = 2;
bool finish = false;

thread *t;
Car *cars;

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
			if (rand() % 100 < 30)
			{
				usleep(rand() % 5000000);
				move(car.getID(), i + 60);
				printw("%c", car.pitstop());
			}
			refresh();
		}
	}
}

bool checkParameters(int argc, char **argv)
{
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

	if (amountOfLaps > 30)
	{
		printf("Max amount of laps is 30!\n");
		endwin();
		return 0;
	}
	return 1;
}

void startRace()
{
	for (int i = 0; i < amountOfCars; i++)
	{
		move(i, 0);
		printw("Car %i:", i);
		move(i, 50);
		printw("pitstop's:", i);
	}

	for (int i = 0; i < amountOfCars; i++)
	{
		Car car(i);
		cars[i] = car;
		t[i] = thread(run, car);
	}

	for (int i = 0; i < amountOfCars; i++)
	{
		t[i].join();
	}
}
int main(int argc, char **argv)
{
	initscr();
	srand(time(NULL));
	if (!checkParameters(argc, argv))
		return 0;

	t = new thread[amountOfCars];
	cars = new Car[amountOfCars];
	refresh();
	startRace();
	for (int i = 0; i < amountOfCars; i++)
	{
		cars[i].~Car();
	}
	delete cars, t;
	getch();
	endwin();

	return 0;
}
