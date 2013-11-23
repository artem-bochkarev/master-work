#pragma once
#include "boost/assert.hpp"

namespace Tools
{
	inline void setBit(size_t& value, size_t pos)
	{
		BOOST_ASSERT_MSG(pos < 32, "Bit number should be < 32");
		value |= (1 << pos);
	}

	inline void clearBit(size_t& value, size_t pos)
	{
		BOOST_ASSERT_MSG(pos < 32, "Bit number should be < 32");
		value &= ~(1 << pos);
	}

	inline void toogleBit(size_t& value, size_t pos)
	{
		BOOST_ASSERT_MSG(pos < 32, "Bit number should be < 32");
		value ^= (1 << pos);
	}

	inline size_t checkBit(size_t value, size_t pos)
	{
		BOOST_ASSERT_MSG(pos < 32, "Bit number should be < 32");
		return value & (1 << pos);
	}

	inline size_t twoPow(size_t power)
	{
		BOOST_ASSERT_MSG(power < 32, "Bit number should be < 32");
		return 1 << power;
	}
}