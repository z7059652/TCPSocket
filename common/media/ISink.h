#pragma once
#include <memory>
#include <stdexcept>
#include "Pipe.h"

class ISink
{
	friend class ISource;
public:
	ISink();
	virtual ~ISink();
protected:
	virtual void BeLinked(const ISource &source);
	std::shared_ptr<Pipe> _in;
};

