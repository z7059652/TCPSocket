#include "Pipe.h"

Pipe::Pipe(size_t size)
	: _cancelled(false)
	, _size(size)
{
}

Pipe::~Pipe()
{
	Cancel();
}

void Pipe::Push(Stream stream)
{
	std::unique_lock<std::mutex> lock(_mutex);
	_cv.wait(lock, [this] {return _cancelled || _queue.size() < _size; });
	_queue.push(stream);
	_cv.notify_one();
}

Stream Pipe::Pop()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_cv.wait(lock, [this] {return _cancelled || !_queue.empty(); });
	Stream stream = _queue.front();
	_queue.pop();
	_cv.notify_one();
	return stream;
}

void Pipe::Cancel()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_cancelled = true;
	_cv.notify_all();
}