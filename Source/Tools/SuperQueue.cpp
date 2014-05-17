#include "SuperQueue.h"

SuperQueue::SuperQueue()
{
	value = 0;
}
boost::uint32_t SuperQueue::getNext()
{
	boost::uint32_t oldVal;
	boost::uint32_t newVal;
	do
	{
		oldVal = value;
		newVal = oldVal + 1;
	} while (oldVal != boost::interprocess::ipcdetail::atomic_cas32(&value, newVal, oldVal));
	return newVal;
}
boost::uint32_t SuperQueue::getNextN(boost::uint32_t n)
{
	boost::uint32_t oldVal;
	boost::uint32_t newVal;
	do
	{
		oldVal = value;
		newVal = oldVal + n;
	} while (oldVal != boost::interprocess::ipcdetail::atomic_cas32(&value, newVal, oldVal));
	return newVal;
}
void SuperQueue::reset()
{
	value = 0;
}
