/***********************************************************************************************
created: 		2023-01-31

author:			chensong

purpose:		csrtp

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
#include "csrtp_util.h"
#include <vector>

namespace chen
{
	// clang-format off
	static const  std::vector<const char*> g_srtp_global_errors =
	{
		// From 0 (srtp_err_status_ok) to 24 (srtp_err_status_pfkey_err).
		"success (srtp_err_status_ok)",
		"unspecified failure (srtp_err_status_fail)",
		"unsupported parameter (srtp_err_status_bad_param)",
		"couldn't allocate memory (srtp_err_status_alloc_fail)",
		"couldn't deallocate memory (srtp_err_status_dealloc_fail)",
		"couldn't initialize (srtp_err_status_init_fail)",
		"can’t process as much data as requested (srtp_err_status_terminus)",
		"authentication failure (srtp_err_status_auth_fail)",
		"cipher failure (srtp_err_status_cipher_fail)",
		"replay check failed (bad index) (srtp_err_status_replay_fail)",
		"replay check failed (index too old) (srtp_err_status_replay_old)",
		"algorithm failed test routine (srtp_err_status_algo_fail)",
		"unsupported operation (srtp_err_status_no_such_op)",
		"no appropriate context found (srtp_err_status_no_ctx)",
		"unable to perform desired validation (srtp_err_status_cant_check)",
		"can’t use key any more (srtp_err_status_key_expired)",
		"error in use of socket (srtp_err_status_socket_err)",
		"error in use POSIX signals (srtp_err_status_signal_err)",
		"nonce check failed (srtp_err_status_nonce_bad)",
		"couldn’t read data (srtp_err_status_read_fail)",
		"couldn’t write data (srtp_err_status_write_fail)",
		"error parsing data (srtp_err_status_parse_err)",
		"error encoding data (srtp_err_status_encode_err)",
		"error while using semaphores (srtp_err_status_semaphore_err)",
		"error while using pfkey (srtp_err_status_pfkey_err)"
	};
	const char * get_srtp_error_desc(srtp_err_status_t code)
	{
		return g_srtp_global_errors.at(code);
	}
}