#include "Pitstop.h"

Pitstop::Pitstop(int _id)
{
	id = _id;
}

Pitstop::Pitstop()
{
}

int Pitstop::getID()
{
	return id;
}

bool Pitstop::getAvailable()
{
	return available;
}

void Pitstop::setAvailable(bool _available)
{
	available = _available;
}