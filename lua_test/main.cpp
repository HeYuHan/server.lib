#include <stdio.h>
extern "C"
{
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h> 

}
#include <LuaEngine.h>

using namespace luabridge;
using namespace Core;

#include <log.h>

class LuaTest :public LuaInterface
{
	// Í¨¹ý LuaInterface ¼Ì³Ð
public:
	virtual const char * GetNativeTypeName() override
	{
		return "LuaTest";
	}
	virtual const char* GetRequireScript() 
	{ 
		return "test"; 
	};
public:
	int intData;
	void Say(const char* msg)
	{
		log_info("luat test say: %s", msg);
	}
	const char* getClassName()
	{
		return "Cpp.LuaTest";
	}
	int testData;
	void SetTestData(int t)
	{
		testData = t;
	}
	int GetTestData() const
	{
		return testData;
	}
};

int RegLua(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<LuaTest>("LuaTest")
		.addData("intData", &LuaTest::intData)
		.addFunction("GetClassName",&LuaTest::getClassName)
		.addProperty("testData",&LuaTest::GetTestData,&LuaTest::SetTestData)
		.endClass();
	return 0;
}
void test()
{
	LuaTest t;
	CallLua<LuaTest*>(&t);
}
int main(int argc, char **args)
{
	LuaEngine::GetInstance()->RegisterModel(RegLua);
	LuaEngine::GetInstance()->Start();
	test();
	return 1;
}