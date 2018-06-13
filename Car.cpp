#include "Car.h"
#include <unistd.h>
#include <ncurses.h>

mutex mu;
condition_variable cond;

Car::Car()
{
}

Car::Car(int _id, short int _color)
{
	id = _id;
	color = _color;
	generateTrace();
}

void Car::generateTrace()
{
	trace.push_back({5, 67, true});
	trace.push_back({6, 68, true});
	trace.push_back({7, 69, true});
	trace.push_back({8, 70, true});
	trace.push_back({9, 71, true});
	trace.push_back({10, 72, true});
	trace.push_back({11, 72, true});
	trace.push_back({12, 72, true});
	trace.push_back({13, 72, true});
	trace.push_back({14, 72, true});
	trace.push_back({15, 72, true});
	trace.push_back({16, 71, true});
	trace.push_back({17, 70, true});
	trace.push_back({18, 69, true});
	trace.push_back({19, 68, true});
	trace.push_back({20, 67, true});
	for (int i = 66; i > 19; i--)
	{
		trace.push_back({20, i, true});
	}
	trace.push_back({19, 19, true});
	trace.push_back({18, 18, true});
	trace.push_back({17, 17, true});
	trace.push_back({16, 16, true});
	trace.push_back({15, 15, true});
	trace.push_back({14, 15, true});
	trace.push_back({13, 15, true});
	trace.push_back({12, 15, true});
	trace.push_back({11, 15, true});
	trace.push_back({10, 16, true});
	trace.push_back({9, 15, true});
	trace.push_back({8, 15, true});
	trace.push_back({7, 16, true});
	trace.push_back({6, 17, true});
	trace.push_back({5, 18, true});
	trace.push_back({4, 19, true});
	for (int i = 19; i < 67; i++)
	{
		trace.push_back({4, i, true});
	}
}

void Car::refueling()
{
	pitstops++;
	actualState = "refueling";
	fuelWarning = 0;
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

	for (int i = 0; i < trace.size(); i++)
	{
		mu.lock();
		attron(COLOR_PAIR(4));
		mvprintw(this->y, this->x, "%s", " ");
		mvprintw(12, 35, "%s", " ");
		mvprintw(12, 52, "%s", " ");
		attroff(COLOR_PAIR(4));
		//refresh();
		mu.unlock();
		y = trace[i].y;
		x = trace[i].x;
		refreshStatus(100 - (i * 10));
		usleep(rand() % 40000 + 30000);
	}
}

void Car::refreshStatus(int percent)
{
	mu.lock();
	if (this->actualState == "driving")
	{
		attron(COLOR_PAIR(color));
		mvprintw(this->y, this->x, "%d", this->id);
		attroff(COLOR_PAIR(color));
	}
	if (this->actualState == "refueling")
	{
		attron(COLOR_PAIR(color));
		mvprintw(12, 35, "%d", this->id);
		attroff(COLOR_PAIR(color));
	}
	if (this->actualState == "in pitlane")
	{
		attron(COLOR_PAIR(color));
		mvprintw(12, 52, "%d", this->id);
		attroff(COLOR_PAIR(color));
	}

	// mvprintw(this->y, 44, "%d", pitstops);
	// mvprintw(2 + id * 2, 15, "          ");
	// mvprintw(2 + id * 2, 15, "%s", actualState.c_str());
	// mvprintw(2 + id * 2, 30, "%d %s", percent, "%");
	// mvprintw(2 + id * 2, 50, "%d", laps);

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