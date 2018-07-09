#pragma once
#ifndef __MESSAGE_QUEUE_H__
#define __MESSAGE_QUEUE_H__
#include "common.h"
BEGIN_NS_CORE
class IThreadMessageHadle
{
	friend class MainThreadMessageQueue;
protected:
	virtual void OnThreadMessage(unsigned int id)=0;
public:
	void SendMessageToMainThread(unsigned int id);
};
class MainThreadMessageQueue
{
	friend class IThreadMessageHadle;
private:
	void RegisterHandle(IThreadMessageHadle *handle, unsigned int id);
public:
	void Update();
	static void Init();
};

END_NS_CORE



#endif // !__MESSAGE_QUEUE_H__
