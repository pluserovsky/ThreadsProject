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
	for (int i = 41; i < 62; i++)
	{
		trace.push_back({10, i, true});
	}
	trace.push_back({10, 62, true});
	trace.push_back({9, 63, true});
	trace.push_back({8, 64, true});
	trace.push_back({6, 65, true});
	trace.push_back({6, 66, true});
	trace.push_back({6, 67, true});
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
	trace.push_back({19, 67, true});
	for (int i = 66; i > 19; i--)
	{
		trace.push_back({19, i, true});
	}
	trace.push_back({19, 19, true});
	trace.push_back({18, 18, true});
	trace.push_back({17, 17, true});
	trace.push_back({16, 16, true});
	trace.push_back({15, 15, true});
	trace.push_back({14, 14, true});
	trace.push_back({13, 14, true});
	trace.push_back({12, 14, true});
	trace.push_back({11, 14, true});
	trace.push_back({10, 14, true});
	trace.push_back({9, 15, true});
	trace.push_back({8, 15, true});
	trace.push_back({7, 16, true});
	trace.push_back({6, 17, true});
	trace.push_back({6, 18, true});
	trace.push_back({6, 19, true});
	trace.push_back({6, 20, true});
	trace.push_back({6, 22, true});
	trace.push_back({6, 23, true});
	trace.push_back({7, 24, true});
	trace.push_back({8, 25, true});
	trace.push_back({9, 26, true});
	trace.push_back({10, 27, true});
	for (int i = 28; i < 40; i++)
	{
		trace.push_back({10, i, true});
	}
}

void Car::refueling()
{
	pitstops++;
	actualState = "refueling";
	fuelWarning = 0;
	mu.lock();
	attron(COLOR_PAIR(4));
	mvprintw(this->y, this->x, "%s", " ");
	attroff(COLOR_PAIR(4));
	mu.unlock();
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
	actualState = "driving";
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
		attroff(COLOR_PAIR(4));
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
		attron(COLOR_PAIR(4));
		mvprintw(12, 29 + this->id, "%s", " ");
		mvprintw(12, 46 + this->id, "%s", " ");
		attroff(COLOR_PAIR(4));
		attron(COLOR_PAIR(color));
		mvprintw(this->y, this->x, "%d", this->id + 1);
		attroff(COLOR_PAIR(color));
	}
	if (this->actualState == "stop")
	{
		attron(COLOR_PAIR(color));
		mvprintw(this->y, this->x, "%d", this->id + 1);
		attroff(COLOR_PAIR(color));
	}
	if (this->actualState == "refueling")
	{
		attron(COLOR_PAIR(color));
		mvprintw(12, 46 + this->id, "%d", this->id + 1);
		attroff(COLOR_PAIR(color));
	}
	if (this->actualState == "tires")
	{
		attron(COLOR_PAIR(color));
		mvprintw(12, 29 + this->id, "%d", this->id + 1);
		attroff(COLOR_PAIR(color));
	}
	attron(COLOR_PAIR(color));
	;
	mvprintw(2 + id, 85, "%d", pitstops);
	mvprintw(2 + id, 95, "%d", laps);
	attron(COLOR_PAIR(color));
	refresh();
	mu.unlock();
}

void Car::tiresPitstop(mutex *mutx)
{
	actualState = "tires";
	refreshStatus(0);
	usleep(100000);
	waitForRefueling = true;
	attron(COLOR_PAIR(4));
	mvprintw(10, 39, "%s", " ");
	attroff(COLOR_PAIR(4));
	for (int i = 0; i < 10; i++)
	{
		refreshStatus(i * 10);
		usleep(rand() % 400000 + 200000);
	}
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
	mu.lock();

	attron(COLOR_PAIR(4));
	mvprintw(10, 39, "%s", " ");
	mvprintw(12, 29 + this->id, "%s", " ");
	attroff(COLOR_PAIR(4));
	mu.unlock();
}

bool Car::twoPitstopsEmpty()
{
	return nextPitstop->getAvailable() && prevPitstop->getAvailable();
}

int Car::getID()
{
	return id;
}
int Car::getY()
{
	return y;
}
int Car::getX()
{
	return x;
}
int Car::getLaps()
{
	return laps;
}
void Car::setPrevPitstop(Pitstop *Prev)
{
	prevPitstop = Prev;
}

void Car::setNextPitstop(Pitstop *Next)
{
	nextPitstop = Next;
}