#ifndef KFIFO_H
#define KFIFO_H

#include <vector>

#define MIN(a, b)	((a) > (b) ? (b) : (a))

static inline unsigned int roundup_power_of_two(unsigned int i)
{
	i--;
	
	i |= i >> 1;
	i |= i >> 2;
	i |= i >> 4;
	i |= i >> 8;
	i |= i >> 16;
	
	return i + 1;
}

static inline bool isPowerOfTwo(unsigned int i)
{
	return ((i != 0) && ((i & (i - 1)) == 0));
}

template <typename T>
class kfifo
{
public:
	kfifo(unsigned int size)
	{
		if (isPowerOfTwo(size))
		{
			buffer_.assign(size, T());
			size_ = size;
		}
		else
		{
			unsigned int size_ = roundup_power_of_two(size);
			buffer_.assign(size_, T());
		}
		writeIndex_ = 0;
		readIndex_ = 0;
	}
	~kfifo(){}
	
	bool push(const T& elem)
	{
		if ((size_ - writeIndex_ + readIndex_) < 1)
			return false;

		__sync_synchronize();

		buffer_[writeIndex_ & (size_ - 1)] = elem;

		__sync_synchronize();
		++writeIndex_;

		return true;
	}
	bool pop(T& elem)
	{
		if ((writeIndex_ - readIndex_) < 1)
			return false;

		__sync_synchronize();

		elem = buffer_[readIndex_ & (size_ - 1)];

		__sync_synchronize();
		++readIndex_;

		return true;
	}
private:
	std::vector<T> buffer_;
	unsigned int size_;
	unsigned int writeIndex_;
	unsigned int readIndex_;
};

#endif
