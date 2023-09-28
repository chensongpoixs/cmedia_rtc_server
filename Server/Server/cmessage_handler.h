/********************************************************************
created:	2021-11-24

author:		chensong

purpose:	Location

*********************************************************************/
#ifndef  _MESSAGE_HANDLER_H_
#define  _MESSAGE_HANDLER_H_

#include <utility>

#include "cconstructor_magic.h"

namespace chen 
{
	class Message;
	class MessageHandler {
		_DISALLOW_COPY_AND_ASSIGN(MessageHandler);
	public:
		virtual ~MessageHandler();
		virtual void OnMessage(Message* msg) = 0;

	protected:
		MessageHandler() {}

		
	};
}// namespace chen

#endif  // RTC_BASE_MESSAGE_HANDLER_H_
