#include "Car.h"
#include <unistd.h>
#include <ncurses.h>

mutex mu;
condition_variable cond;

Car::Car()
{
}

Car::Car(int _id)
{
	id = _id;
}

void Car::refueling()
{
	pitstops++;
	actualState = "refueling";
	fuelWarning = 0;;
	for (int i = 0; i < 10; i++)
	{
		refreshStatus(i * 10);
		usleep(rand() % 400000 + 200000);
	}
	unique_lock<mutex> locker(mu);
	prevPitstop->setAvailable(true);
	nextPitstop->setAvailable(true);
	cond.notify_all();
	locker.unlock();
	refreshStatus(0);
}

void Car::driving()
{
	laps++;
	actualState = "driving";
	fuelWarning++;
	for (int i = 0; i < 10; i++)
	{
		refreshStatus(100-(i * 10));
		usleep(rand() % 400000 + 300000);
	}
}

void Car::refreshStatus(int percent)
{
	mu.lock();
	mvprintw(2 + id * 2, 44, "%d", pitstops);
	mvprintw(2 + id * 2, 15, "          ");
	mvprintw(2 + id * 2, 15, "%s", actualState.c_str());
	mvprintw(2 + id * 2, 30, "%d %s", percent, "%");
	mvprintw(2 + id * 2, 50, "%d", laps);

	refresh();
	mu.unlock();
}

void Car::signalPitstop(mutex *mutx)
{
	actualState = "in pitlane";
	refreshStatus(0);
	usleep(100000);
	waitForRefueling = true;
	while (waitForRefueling)
	{
		unique_lock<mutex> locker(mu);
		cond.wait(locker, [&] { return twoPitstopsEmpty(); });
		if (twoPitstopsEmpty())
		{
			if (pitstopFlag)
			{
				mutx->unlock();
				pitstopFlag = false;
			}
			nextPitstop->setAvailable(false);
			prevPitstop->setAvailable(false);
			waitForRefueling = false;
			locker.unlock();
		}
	}
}

bool Car::twoPitstopsEmpty()
{
	return nextPitstop->getAvailable() && prevPitstop->getAvailable();
}

int Car::getID()
{
	return id;
}

void Car::setPrevPitstop(Pitstop *Prev)
{
	prevPitstop = Prev;
}

void Car::setNextPitstop(Pitstop *Next)
{
	nextPitstop = Next;
}