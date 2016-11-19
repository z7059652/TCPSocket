#include "ISource.h"
#include "ISink.h"
#include "IFilter.h"

ISource::~ISource()
{
}

void ISource::operator>>(ISink &sink)
{
	_out = std::make_shared<Pipe>(4);
	sink.BeLinked(*this);
}

ISource &ISource::operator>>(IFilter &filter)
{
	operator>>((ISink&)filter);
	return filter;
}