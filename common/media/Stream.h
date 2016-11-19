#pragma once
#include <vector>
#include <memory>
class Stream
{
public:
	Stream(size_t size = 1);
	virtual ~Stream();

	std::shared_ptr<unsigned char> buffer;
	int m_bufferSize;
};

