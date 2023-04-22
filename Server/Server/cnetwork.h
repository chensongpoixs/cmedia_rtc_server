/***********************************************************************************************
created: 		2022-12-27

author:			chensong

purpose:		Íø¿¨ÐÅÏ¢
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

************************************************************************************************/

#ifndef _C_NETWORK_H_
#define _C_NETWORK_H_
#include "cnet_type.h"
#include "cip_address.h"
#include "cnetwork_constants.h"
#include "csocket_util.h"
#include <vector>
namespace chen {

	class cnetwork
	{
	public:
		explicit cnetwork(const std::string& name,
			const std::string& description,
			const cip_address& prefix,
			int32 prefix_length)
			: m_name(name)
			, m_description(description)
			, m_prefix(prefix)
			, m_prefix_length(prefix_length)
			, m_key(MakeNetworkKey(name, prefix, prefix_length))
			, m_ips()
			, m_type(ADAPTER_TYPE_UNKNOWN)
			, m_scope_id(0)
			, m_ignored(false)
		{}

		explicit cnetwork(const std::string& name,
			const std::string& description,
			const cip_address& prefix,
			int32 prefix_length,
			AdapterType type) 
			: m_name(name)
			, m_description(description)
			, m_prefix(prefix)
			, m_prefix_length(prefix_length)
			, m_key(MakeNetworkKey(name, prefix, prefix_length))
			, m_ips()
			, m_type(type)
			, m_scope_id(0)
			, m_ignored(false) {}


		

		~cnetwork() {}
	public:

		// Returns the name of the interface this network is associated with.
		const std::string& name() const { return m_name; }

		// Returns the OS-assigned name for this network. This is useful for
		// debugging but should not be sent over the wire (for privacy reasons).
		const std::string& description() const { return m_description; }

		// Returns the prefix for this network.
		const cip_address& prefix() const { return m_prefix ; }
		// Returns the length, in bits, of this network's prefix.
		int prefix_length() const { return m_prefix_length ; }

		// |key_| has unique value per network interface. Used in sorting network
		// interfaces. Key is derived from interface name and it's prefix.
		std::string key() const { return m_key ; }

		// Returns the Network's current idea of the 'best' IP it has.
		// Or return an unset IP if this network has no active addresses.
		// Here is the rule on how we mark the IPv6 address as ignorable for WebRTC.
		// 1) return all global temporary dynamic and non-deprecated ones.
		// 2) if #1 not available, return global ones.
		// 3) if #2 not available, use ULA ipv6 as last resort. (ULA stands
		// for unique local address, which is not route-able in open
		// internet but might be useful for a close WebRTC deployment.

		// TODO(guoweis): rule #3 actually won't happen at current
		// implementation. The reason being that ULA address starting with
		// 0xfc 0r 0xfd will be grouped into its own Network. The result of
		// that is WebRTC will have one extra Network to generate candidates
		// but the lack of rule #3 shouldn't prevent turning on IPv6 since
		// ULA should only be tried in a close deployment anyway.

		// Note that when not specifying any flag, it's treated as case global
		// IPv6 address
		cip_address GetBestIP() const;

		// Keep the original function here for now.
		// TODO(guoweis): Remove this when all callers are migrated to GetBestIP().
		cip_address ip() const { return GetBestIP(); }


		// Adds an active IP address to this network. Does not check for duplicates.
		void AddIP(const cinterface_address& ip) { m_ips.push_back(ip); }
		void AddIP(const cip_address& ip) { m_ips.push_back(cinterface_address(ip)); }

		// Sets the network's IP address list. Returns true if new IP addresses were
		// detected. Passing true to already_changed skips this check.
		bool SetIPs(const std::vector<cinterface_address>& ips, bool already_changed);
		// Get the list of IP Addresses associated with this network.
		const std::vector<cinterface_address>& GetIPs() const { return m_ips; }
		// Clear the network's list of addresses.
		void ClearIPs() { m_ips.clear(); }

		// Returns the scope-id of the network's address.
		// Should only be relevant for link-local IPv6 addresses.
		int scope_id() const { return m_scope_id; }
		void set_scope_id(int id) { m_scope_id = id; }

		// Indicates whether this network should be ignored, perhaps because
		 // the IP is 0, or the interface is one we know is invalid.
		bool ignored() const { return m_ignored; }
		void set_ignored(bool ignored) { m_ignored = ignored; }
	private:
		cnetwork(const cnetwork&);
	protected:
	private:
		std::string						m_name;
		std::string						m_description;
		cip_address						m_prefix;
		int32							m_prefix_length;
		std::string						m_key;
		std::vector<cinterface_address> m_ips;
		AdapterType						m_type;
		int32							m_scope_id;
		bool							m_ignored;
	};

}

#endif 