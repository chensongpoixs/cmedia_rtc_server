#include "crandom.h"
#include "ctime_api.h"

namespace chen
{
	crandom::crandom()
	{
		init(static_cast<uint32>(ctime_base_api::get_gmt()));
	}

	crandom::crandom(uint32 seed)
	{
		init(seed);
	}

	void crandom::init()
	{

	}

	void crandom::init(uint32 seed)
	{
		m_index = 0;
		m_MT[0] = seed;
		for (uint32 i = 1; i < MAX_SEED_NUM; ++i)
		{
			m_MT[i] = 0x6C078965 * (m_MT[i - 1] ^ (m_MT[i - 1] >> 30)) + i;
		}
	}

	uint32 crandom::rand()
	{
		uint32 y = 0;
		if (m_index >= MAX_SEED_NUM)
		{
			for (uint32 i = 0; i < MAX_SEED_NUM; ++i)
			{
				y = (m_MT[i] & 0x80000000) + (m_MT[(i + 1) % MAX_SEED_NUM] & 0x7fffffff);
				m_MT[i] = m_MT[(i + 397) % MAX_SEED_NUM] ^ (y >> 1);
				if (y % 2)
				{ /* y is odd */
					m_MT[i] = m_MT[i] ^ (0x9908B0DF);
				}
			}
			m_index = 0;
		}

		y = m_MT[m_index++];

		y = y ^ (y >> 11);
		y = y ^ ((y << 7) & 0x9D2C5680);
		y = y ^ ((y << 15) & 0xEFC60000);
		y = y ^ (y >> 18);

		return y;
	}

	uint32 crandom::rand(uint32 minValue, uint32 maxValue)
	{
		if (minValue < maxValue)
		{
			return	(rand()) % (maxValue - minValue + 1) + minValue;
		}
		else if (minValue > maxValue)
		{
			return	(rand()) % (minValue - maxValue + 1) + maxValue;
		}
		else
		{
			return minValue;
		}
	}
	uint32 crandom::rand(uint32 maxValue)
	{
		if (maxValue != 0)
		{
			return rand() % maxValue;
		}
		return 0;
	}

	std::string crandom::rand_str(int32 len)
	{
		static std::string random_table = "01234567890123456789012345678901234567890123456789abcdefghijklmnopqrstuvwxyz";

		std::string ret;
		ret.reserve(len);
		for (int32 i = 0; i < len; ++i) 
		{
			ret.append(1, random_table[rand() % random_table.size()]);
		}

		return ret;
	}

	float crandom::randf()
	{
		return (rand() % 100000 + 1) / 100000.f;
	}
}