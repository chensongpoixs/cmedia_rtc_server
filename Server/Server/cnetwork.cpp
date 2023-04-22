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
#include "cnetwork.h"
#include <unordered_map>
#include <unordered_set>
namespace chen {

	namespace container_algorithm_internal {

		// NOTE: it is important to defer to ADL lookup for building with C++ modules,
		// especially for headers like <valarray> which are not visible from this file
		// but specialize std::begin and std::end.
		using std::begin;
		using std::end;

		// The type of the iterator given by begin(c) (possibly std::begin(c)).
		// ContainerIter<const vector<T>> gives vector<T>::const_iterator,
		// while ContainerIter<vector<T>> gives vector<T>::iterator.
		template <typename C>
		using ContainerIter = decltype(begin(std::declval<C&>()));

		// An MSVC bug involving template parameter substitution requires us to use
		// decltype() here instead of just std::pair.
		template <typename C1, typename C2>
		using ContainerIterPairType =
			decltype(std::make_pair(ContainerIter<C1>(), ContainerIter<C2>()));

		template <typename C>
		using ContainerDifferenceType =
			decltype(std::distance(std::declval<ContainerIter<C>>(),
				std::declval<ContainerIter<C>>()));

		template <typename C>
		using ContainerPointerType =
			typename std::iterator_traits<ContainerIter<C>>::pointer;

		// container_algorithm_internal::c_begin and
		// container_algorithm_internal::c_end are abbreviations for proper ADL
		// lookup of std::begin and std::end, i.e.
		//   using std::begin;
		//   using std::end;
		//   std::foo(begin(c), end(c);
		// becomes
		//   std::foo(container_algorithm_internal::begin(c),
		//   container_algorithm_internal::end(c));
		// These are meant for internal use only.

		template <typename C>
		ContainerIter<C> c_begin(C& c) { return begin(c); }

		template <typename C>
		ContainerIter<C> c_end(C& c) { return end(c); }

		template <typename T>
		struct IsUnorderedContainer : std::false_type {};

		template <class Key, class T, class Hash, class KeyEqual, class Allocator>
		struct IsUnorderedContainer<
			std::unordered_map<Key, T, Hash, KeyEqual, Allocator>> : std::true_type {};

		template <class Key, class Hash, class KeyEqual, class Allocator>
		struct IsUnorderedContainer<std::unordered_set<Key, Hash, KeyEqual, Allocator>>
			: std::true_type {};

	}  // namespace container_algorithm_internal


	// Performs a linear search for `value` using the iterator `first` up to
// but not including `last`, returning true if [`first`, `last`) contains an
// element equal to `value`.
//
// A linear search is of O(n) complexity which is guaranteed to make at most
// n = (`last` - `first`) comparisons. A linear search over short containers
// may be faster than a binary search, even when the container is sorted.
	template <typename InputIterator, typename EqualityComparable>
	bool linear_search(InputIterator first, InputIterator last,
		const EqualityComparable& value) {
		return std::find(first, last, value) != last;
	}
	// c_linear_search()
//
// Container-based version of absl::linear_search() for performing a linear
// search within a container.
	template <typename C, typename EqualityComparable>
	bool c_linear_search(const C& c, EqualityComparable&& value) {
		return linear_search(container_algorithm_internal::c_begin(c),
			container_algorithm_internal::c_end(c),
			std::forward<EqualityComparable>(value));
	}

	// Select the best IP address to use from this Network.
	cip_address cnetwork::GetBestIP() const
	{
		if (m_ips.size() == 0) {
			return cip_address();
		}

		if (m_prefix.family() == AF_INET) {
			return static_cast<cip_address>(m_ips.at(0));
		}

		cinterface_address selected_ip, ula_ip;

		for (const cinterface_address& ip : m_ips) {
			// Ignore any address which has been deprecated already.
			if (ip.ipv6_flags() & IPV6_ADDRESS_FLAG_DEPRECATED)
			{
				continue;
			}

			// ULA address should only be returned when we have no other
			// global IP.
			if (IPIsULA(static_cast<const cip_address&>(ip))) {
				ula_ip = ip;
				continue;
			}
			selected_ip = ip;

			// Search could stop once a temporary non-deprecated one is found.
			if (ip.ipv6_flags() & IPV6_ADDRESS_FLAG_TEMPORARY)
				break;
		}

		// No proper global IPv6 address found, use ULA instead.
		if (IPIsUnspec(selected_ip) && !IPIsUnspec(ula_ip)) {
			selected_ip = ula_ip;
		}

		return static_cast<cip_address>(selected_ip);
	}
	// Sets the addresses of this network. Returns true if the address set changed.
// Change detection is short circuited if the changed argument is true.
	bool cnetwork::SetIPs(const std::vector<cinterface_address>& ips, bool changed)
	{
		// Detect changes with a nested loop; n-squared but we expect on the order
  // of 2-3 addresses per network.
		changed = changed || ips.size() != m_ips.size();
		if (!changed) 
		{
			for (const cinterface_address& ip : ips)
			{
				if (!c_linear_search(m_ips, ip)) 
				{
					changed = true;
					break;
				}
			}
		}

		m_ips = ips;
		return changed;
	}
	/*cnetwork::cnetwork(const cnetwork &)
	{
	}*/
}