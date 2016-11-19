#pragma once
#include "Stream.h"
#include <queue>
#include <mutex>
#include <condition_variable>

class Pipe
{
public:
	Pipe(size_t size);
	virtual ~Pipe();

	void Push(Stream stream);
	Stream Pop();
	void Cancel();
private:
	std::queue<Stream> _queue;
	std::mutex _mutex;
	std::condition_variable _cv;
	bool _cancelled;
	size_t _size;
};