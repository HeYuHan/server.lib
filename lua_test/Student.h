#include <LuaEngine.h>
class Student:public LuaInterface
{
public:
	Student();
	~Student();

	void Run();
	int t = 0;
	void Run(int a);
private:

	// Í¨¹ý LuaInterface ¼Ì³Ð
	virtual const char * GetRequireScript() override;
	virtual const char * GetNativeTypeName() override;
};