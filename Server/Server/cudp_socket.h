/***********************************************************************************************
created: 		2022-08-12

author:			chensong

purpose:		cudp_socket 

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



UDP主要丢包原因

 
1、接收端处理时间过长导致丢包：接收端调用recv方法收到数据后，处理数据花了一些时间，处理完后再次调用recv方法，在这二次调用间隔里，发过来的包可能丢失。对于这种情况可以修改接收端，将包接收后存入一个缓冲区。

2、发送的包太大导致丢包：虽然send方法会将大包切割成小包然后再发送，但包太大也不行。例如超过50K的一个udp包，不切割直接通过send方法发送也会导致这个包丢失。这种情况需要切割成小包再逐个send。

3、UDP发送的包较大，超过接受者缓存导致丢包：几个大的udp包可能会超过接收者的缓冲，导致丢包。这种情况可以设置socket接收缓冲。比如将接收缓冲增加到64K。

4、发送包的频率太快：即使每个包的大小都小于接收缓冲，但是如果频率太快中间不sleep，也有可能导致丢包。这种情况有时通过设置socket接收缓冲可以解决，但有时解决不了。解决方法就是减小发送频率。

5、局域网内不丢包，公网上丢包。这个问题也可以通过对packet进行切割并sleep来解决。但是如果网络流量太大，这个办法也不能保证不丢包。另外还有这几种方法来解决丢包： 减小流量、换tcp协议传输或者做丢包重传的工作。
************************************************************************************************/


#ifndef _C_UDP_SOCKET_H_
#define _C_UDP_SOCKET_H_

#include "cudp_socket_handler.h"

namespace chen {
	class cudp_socket : public cudp_socket_handler
	{
	public:
		class Listener
		{
		public:
		//	virtual ~Listener() = default;

		public:
			virtual void OnUdpSocketPacketReceived(
				cudp_socket* socket, const uint8_t* data, size_t len, const struct sockaddr* remoteAddr) = 0;
		};
	public:
		cudp_socket(Listener* listener, std::string& ip );
		cudp_socket(Listener* listener, std::string& ip, uint16_t port);
		~cudp_socket();

	public:
		/* Pure virtual methods inherited from ::UdpSocketHandler. */
		void UserOnUdpDatagramReceived(const uint8_t* data, size_t len, const struct sockaddr* addr)  override;
	private:
		// Passed by argument.
		Listener*			m_listener;// { nullptr };
		bool				m_fixedPort;// { false };
	};
		
}

#endif // _C_UDP_SOCKET_H_