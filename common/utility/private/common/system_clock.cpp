#include "system_clock.h"

using namespace std::chrono;
using namespace Titanium::TIRA;

ti_timestamp ti_system_clock::now_as_timestamp()
{
	ti_timestamp rt = { time_point_cast<nanoseconds>(high_resolution_clock::now()).time_since_epoch().count() / 100 };
	return rt;
}

long long ti_system_clock::now_as_hnano()
{
	return now_as_timestamp().as_hnano();
}

long long ti_system_clock::now_as_micro()
{
	return now_as_timestamp().as_micro();
}

long long ti_system_clock::now_as_milli()
{
	return now_as_timestamp().as_milli();
}