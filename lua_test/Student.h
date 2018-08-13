#include <LuaEngine.h>
#include <common.h>
#include <log.h>
USING_NS_CORE;
class Student:public LuaInterface
{
public:
	Student();
	~Student();
	void Run(int a);
	void PrintArg() { log_debug("arg=%d", arg); }
public:
	int arg;
private:

	// ͨ�� LuaInterface �̳�
	virtual const char * GetRequireScript() override;
	virtual const char * GetNativeTypeName() override;
};