#include "Car.h"

using namespace std;

int amountOfCars = 9, lapsAmount = 50;
mutex carMutex, checkMutex;
bool finish = false;
char **cityTable;
int **carTable;
fstream file;
int a, b, c, di;

void drawMap()
{

	start_color();
	init_color(COLOR_RED, 139, 69, 19);
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	init_pair(2, COLOR_WHITE, COLOR_GREEN);
	init_pair(3, COLOR_WHITE, COLOR_RED);
	init_pair(4, COLOR_WHITE, COLOR_BLACK);
	init_pair(5, COLOR_WHITE, COLOR_YELLOW);
	init_pair(6, COLOR_WHITE, COLOR_BLUE);
	init_pair(7, COLOR_WHITE, COLOR_MAGENTA);
	init_pair(8, COLOR_WHITE, COLOR_CYAN);
	init_pair(9, COLOR_RED, COLOR_BLUE);

	bkgd(COLOR_PAIR(1));

	for (int i = 0; i < a; i++)
	{
		for (int j = 0; j < b; j++)
		{
			if (cityTable[i][j] == '0')
			{
				move(i, j);
				attron(COLOR_PAIR(4));
				addch(' ');
				attroff(COLOR_PAIR(4));
			}
			else if (cityTable[i][j] == '7')
			{
				move(i, j);
				attron(COLOR_PAIR(2));
				addch(' ');
				attroff(COLOR_PAIR(2));
			}
			else if (cityTable[i][j] == '3')
			{
				move(i, j);
				attron(COLOR_PAIR(3));
				addch(' ');
				attroff(COLOR_PAIR(3));
			}
			else if (cityTable[i][j] == 'B')
			{
				move(i, j);
				attron(COLOR_PAIR(4));
				addch(' ');
				attroff(COLOR_PAIR(4));
			}
			else if (cityTable[i][j] == '5')
			{
				move(i, j);
				attron(COLOR_PAIR(5));
				addch(' ');
				attroff(COLOR_PAIR(5));
			}
			else if (cityTable[i][j] == '1')
			{
				move(i, j);
				attron(COLOR_PAIR(1));
				addch(' ');
				attroff(COLOR_PAIR(1));
			}
			else if (cityTable[i][j] == '8')
			{
				move(i, j);
				attron(COLOR_PAIR(8));
				addch(' ');
				attroff(COLOR_PAIR(8));
			}
			else if (cityTable[i][j] == 'M')
			{
				move(i, j);
				attron(COLOR_PAIR(7));
				addch(' ');
				attroff(COLOR_PAIR(7));
			}
		}
	}
}

void readMapFromFile()
{

	file.open("data.txt", ios::in);
	file >> a;
	file >> b;

	cityTable = new char *[a];
	for (int i = 0; i < a; i++)
	{
		cityTable[i] = new char[b];
		for (int j = 0; j < b; j++)
		{
			file >> cityTable[i][j];
		}
	}
}

void run(Car car)
{
	while (!finish)
	{
		if (car.getLaps() == lapsAmount)
		{
			finish = true;
			attron(COLOR_PAIR(1));
			mvprintw(20, 30, "%s %d", "THE WINNER IS CAR NR ", car.getID() + 1);
			attroff(COLOR_PAIR(1));
			getch();
			break;
		}
		car.driving();
		checkMutex.lock();
		if (car.getWarning() > 2)
		{
			carMutex.lock();
			car.setPitstopFlag(true);
		}
		checkMutex.unlock();
		if (rand() % 5 + 1 < 2)
		{
			car.tiresPitstop(&carMutex);
			car.refueling();
		}
	}
}

int main(int argc, char **argv)
{
	srand(time(NULL));
	thread *t = new thread[amountOfCars];
	Car *cars = new Car[amountOfCars];
	Pitstop *pitstops = new Pitstop[amountOfCars];
	initscr();
	curs_set(0);
	nodelay(stdscr, TRUE);
	readMapFromFile();
	drawMap();
	int colors[9] = {9, 7, 8, 6, 5, 3, 2, 1, 4};
	Pitstop pitstop1(1);
	pitstops[1] = pitstop1;
	Pitstop pitstop2(2);
	pitstops[2] = pitstop2;
	for (int i = 0; i < amountOfCars; i++)
	{
		//Car car(i, (rand() % 5) + 1);
		Car car(i, colors[i]);
		cars[i] = car;
		//Pitstop pitstop(i);
		//pitstops[i] = pitstop;
		//cars[i].setPrevPitstop(&pitstops[i]);
		if (i > 0)
		{
			cars[i].setNextPitstop(&pitstops[i - 1]);
		}
		if (i == amountOfCars - 1)
		{
			cars[0].setNextPitstop(&pitstops[amountOfCars - 1]);
		}
		attron(COLOR_PAIR(2));
		mvprintw(2 + i, 80, "%d", i + 1);
		attroff(COLOR_PAIR(2));
	}
	attron(COLOR_PAIR(2));
	mvprintw(0, 80, "%s  %s %s /%d", "ID", "Pitstops", " Laps", lapsAmount);
	attroff(COLOR_PAIR(2));
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