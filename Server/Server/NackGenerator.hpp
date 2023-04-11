#ifndef MS_RTC_NACK_GENERATOR_HPP
#define MS_RTC_NACK_GENERATOR_HPP

//#include "common.hpp"
#include "RtpPacket.hpp"
#include "SeqManager.hpp"
//#include "handles/Timer.hpp"
#include "ctimer.h"
#include <map>
#include <set>
#include <vector>

namespace RTC
{
	class NackGenerator : public chen::ctimer  ::Listener
	{
	public:
		class Listener
		{
		public:
			virtual ~Listener() = default;

		public:
			virtual void OnNackGeneratorNackRequired(const std::vector<uint16_t>& seqNumbers) = 0;
			virtual void OnNackGeneratorKeyFrameRequired()                                    = 0;
		};

	private:
		struct NackInfo
		{
			NackInfo() = default;
			explicit NackInfo(uint16_t seq, uint16_t sendAtSeq) : seq(seq), sendAtSeq(sendAtSeq)
			{
			}

			uint16_t seq{ 0u };
			uint16_t sendAtSeq{ 0u };
			uint64_t sentAtMs{ 0u };
			uint8_t retries{ 0u };
		};

		enum class NackFilter
		{
			SEQ,
			TIME
		};

	public:
		explicit NackGenerator(Listener* listener);
		~NackGenerator() override;

		bool ReceivePacket(RTC::RtpPacket* packet, bool isRecovered);
		size_t GetNackListLength() const
		{
			return this->nackList.size();
		}
		void UpdateRtt(uint32_t rtt)
		{
			this->rtt = rtt;
		}
		void Reset();

	private:
		void AddPacketsToNackList(uint16_t seqStart, uint16_t seqEnd);
		bool RemoveNackItemsUntilKeyFrame();
		std::vector<uint16_t> GetNackBatch(NackFilter filter);
		void MayRunTimer() ;

		/* Pure virtual methods inherited from Timer::Listener. */
	public:
		void OnTimer(chen::ctimer * timer)  ;

	private:
		// Passed by argument.
		Listener* listener{ nullptr };
		// Allocated by this.
		chen::ctimer* timer{ nullptr };
		// Others.
		std::map<uint16_t, NackInfo, RTC::SeqManager<uint16_t>::SeqLowerThan> nackList;
		std::set<uint16_t, RTC::SeqManager<uint16_t>::SeqLowerThan> keyFrameList;
		std::set<uint16_t, RTC::SeqManager<uint16_t>::SeqLowerThan> recoveredList;
		bool started{ false };
		uint16_t lastSeq{ 0u }; // Seq number of last valid packet.
		uint32_t rtt{ 0u };     // Round trip time (ms).
	};
} // namespace RTC

#endif
