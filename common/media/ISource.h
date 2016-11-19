#pragma once
#include "Pipe.h"
#include <memory>

class ISource
{
	friend class ISink;
	friend class IFilter;
public:
	virtual ~ISource();

	virtual void operator>>(ISink &sink);
	virtual ISource &operator>>(IFilter &sink);
protected:
	std::shared_ptr<Pipe> _out;
};