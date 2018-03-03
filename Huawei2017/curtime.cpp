#include"curtime.h"
#include<ctime>
#include<sys/timeb.h>
#include<random>

unsigned long getCurTime()
{
	struct timeb rawtime;
	struct tm * timeinfo;
	ftime(&rawtime);
	timeinfo = localtime(&rawtime.time);

	int ms = rawtime.millitm;
	unsigned long s = rawtime.time;

	return s * 1000 + ms;
}

int getRandInt(int N)
{
	static std::default_random_engine e(std::time(nullptr));
	static std::uniform_int_distribution<unsigned> u(0, N);
	return u(e);
}

double getRandP()
{
	static std::default_random_engine e(std::time(nullptr));
	static std::uniform_real_distribution<double> u(0, 1);
	return u(e);
}