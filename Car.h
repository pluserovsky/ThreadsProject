#include <iostream>
#include <ncurses.h>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <string>
#include <mutex>
#include <condition_variable>
#include "Pitstop.h"

using namespace std;

static int levelPriority = 0;

class Car
{
	bool waitForRefueling = false;
	int id, pitstops = 0, laps = 0, priority = 0;
	Pitstop *prevPitstop, *nextPitstop;
	Pitstop *pit;
	bool priorityFlag = false;
	string actualState = "driving";

  public:
	Car();
	Car(int _id);
	void run();
	void driving();
	void refueling();
	int getID();
	Pitstop getPrevPitstop();
	Pitstop getRightPitstop();
	void setPrevPitstop(Pitstop *Prev);
	void setNextPitstop(Pitstop *Next);
	void simulatingLife();
	void signalPitstop(mutex *mutx);
	bool needFuel();
	bool twoPitstopsFull();
	void refreshStatus(int percent);
	int getPriority() { return priority; }
	void setPriorityFlag(bool _priority) { priorityFlag = _priority; }
};
