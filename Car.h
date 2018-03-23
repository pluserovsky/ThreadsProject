#include <iostream>
#include <ncurses.h>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <string>

using namespace std;

class Car
{

	int id;
	int laps;

  public:
	Car();
	Car(int _id);
	void run();
	char driving();
	char pitstop();
	int getID();
	int getLaps();
};
