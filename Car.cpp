#include "Car.h"

Car::Car()
{
	//cout << "X" << endl;
	laps = 0;
}

Car::Car(int _id)
{
	id=_id;
	laps = 0;
}

char Car::driving()
{
	//cout<<"Car "<<id<<" is driving.\n";
	return '>';
	laps++;
	//printw("->");
}

char Car::cooling()
{
	//cout<<"Car "<<id<<" is cooling.\n";
	return '|';
	//printw("|");
}

int Car::getID()
{
	return id;
}
int Car::getLaps()
{
	return laps;
}

