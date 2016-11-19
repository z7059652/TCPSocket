#pragma once
#include "ISource.h"
#include "ISink.h"
class IFilter : public ISource, public ISink
{
public:
	virtual ~IFilter();
};

