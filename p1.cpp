#include "Car.h"

using namespace std;


int amountOfCars = 5;
mutex carMutex, checkMutex;
bool finish = false;


void run(Car car)
{
	while (!finish)
	{
		if (getch() == 'q')
		{
			finish = true;
			break;
		}
		car.driving();
		checkMutex.lock();
		if (car.getPriority() > 2)
		{
			carMutex.lock();
			car.setPriorityFlag(true);
		}
		checkMutex.unlock();
		car.signalPitstop(&carMutex);
		car.refueling();
	}
}

int main(int argc, char **argv)
{
	srand(time(NULL));
	thread *t = new thread[amountOfCars];
	Car *cars = new Car[amountOfCars];
	Pitstop *pitstops = new Pitstop[amountOfCars];
	initscr();
	nodelay(stdscr, TRUE);
	start_color();

	for (int i = 0; i < amountOfCars; i++)
	{
		Car car(i);
		cars[i] = car;

		Pitstop fork(i);
		pitstops[i] = fork;

		cars[i].setPrevPitstop(&pitstops[i]);
		if (i > 0)
		{
			cars[i].setNextPitstop(&pitstops[i - 1]);
		}
		if (i == amountOfCars - 1)
		{
			cars[0].setNextPitstop(&pitstops[amountOfCars - 1]);
		}

		mvprintw(2 + i * 2, 0, "%s: %d", "Car", i);
	}
	mvprintw(0, 14, "%s %s %s", "Actual state   Fuel (%)", " Pitstops", " Laps");
	refresh();

	for (int i = 0; i < amountOfCars; i++)
		t[i] = thread(run, cars[i]);

	for (int i = 0; i < amountOfCars; i++)
	{
		t[i].join();
	}
	endwin();

	return 0;
}