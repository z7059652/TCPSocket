#include "ISink.h"
#include "ISource.h"

ISink::ISink()
{
}


ISink::~ISink()
{
}

void ISink::BeLinked(const ISource & source)
{
	_in = source._out;
}
