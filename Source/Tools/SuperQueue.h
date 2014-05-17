#include <boost/interprocess/detail/atomic.hpp>

class SuperQueue
{
public:
	SuperQueue();
	boost::uint32_t getNext();
	boost::uint32_t getNextN(boost::uint32_t n);
	void reset();
private:
	boost::uint32_t value;
};