/***********************************************************************************************
created: 		2019-03-01

author:			chensong

purpose:		net_type

输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
**********************************************************************************/



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