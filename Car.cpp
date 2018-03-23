#include "Car.h"

Car::Car()
{
	laps = 0;
}

Car::Car(int _id)
{
	id = _id;
	laps = 0;
}

char Car::driving()
{
	laps++;
	return '>';
}

char Car::cooling()
{
	return '|';
}

int Car::getID()
{
	return id;
}
int Car::getLaps()
{
	return laps;
}
