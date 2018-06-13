#include <iostream>
#include <ncurses.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <unistd.h>
#include <string>
#include <mutex>
#include <vector>
#include <condition_variable>
#include "Pitstop.h"

using namespace std;

static int fuelWarning = 0;

struct Coordinate
{
	int y, x;
	bool available;
};

class Car
{
	bool waitForRefueling = false;
	int id, pitstops = 0, laps = 0, warning = 0,
			x = 0, y = 0;
	Pitstop *prevPitstop, *nextPitstop;
	Pitstop *pit;
	bool pitstopFlag = false;
	string actualState = "driving";
	short int color;
	vector<Coordinate> trace;
	vector<Coordinate> pitstopTrace;
	void generateTrace();
	void generatePitstopTrace();

  public:
	Car();
	Car(int _id, short int _color);
	void run();
	void driving();
	void refueling();
	int getID();
	int getY();
	int getX();
	Pitstop getPrevPitstop();
	Pitstop getRightPitstop();
	void setPrevPitstop(Pitstop *Prev);
	void setNextPitstop(Pitstop *Next);
	void simulatingLife();
	void signalPitstop(mutex *mutx);
	bool twoPitstopsEmpty();
	void refreshStatus(int percent);
	int getWarning() { return warning; }
	void setPitstopFlag(bool _priority) { pitstopFlag = _priority; }
};
