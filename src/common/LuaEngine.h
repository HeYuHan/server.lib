#pragma once
#ifndef __ILUA_SCRIPT_H__
#define __ILUA_SCRIPT_H__

#include "LuaBridge/LuaFunction.h"
#include "common.h"
#include "log.h"

#define REG_GET_SET(__NAME__) METHOD(Get##__NAME__) METHOD(Set##__NAME__)
BEGIN_NS_CORE
typedef int(*register_lua_model)(lua_State* L);
class LuaInterface
{
	friend class LuaEngine;
public:
	LuaInterface();
	~LuaInterface();
protected:
public:
	virtual const char* GetRequireScript() { return NULL; };
	virtual const char* GetNativeTypeName() = 0;
	virtual const char* GetScriptTypeName() { return NULL; };
	int m_LuaRef;
	lua_State *L;
};


class LuaEngine
{
	friend class LuaInterface;
public:
	~LuaEngine();
public:
	void RegisterModel(register_lua_model model);
	void Start(const char* mainLua=0);
	bool DoFile(const char* file_path);

	bool CreateScriptHandle(LuaInterface *lua);
	void DestoryScriptHandle(LuaInterface *lua);
	void LuaSearchPath(const char *name, const char *value);

	static LuaEngine* GetInstance();
	void Pop(int old_top,const char* msg=0);
private:
	lua_State* L;
	static LuaEngine* m_Instance;
};
template <class T>
inline void CallLua(T t)
{
	if (t->m_LuaRef < 0)
	{
		LuaEngine::GetInstance()->CreateScriptHandle(t);
	}
	if (t->m_LuaRef > 0 && t->L)
	{
		int top = lua_gettop(t->L);
		lua_rawgeti(t->L, LUA_REGISTRYINDEX, t->m_LuaRef);
		lua_pushnil(t->L);
		luabridge::push<T>(t->L, t);
		int error = lua_pcall(t->L, 2, 0, 0);
		if (error != LUA_OK)
		{
			log_error("%s", lua_tostring(t->L, -1));
		}
		LuaEngine::GetInstance()->Pop(top);
	}
	else
	{
		log_error("%s", "native object not have luaref");
	}
}
END_NS_CORE
#endif // !__ILUA_SCRIPT_H__

//////////////////////////////////////////////////////////////////////////
//sample
//class LuaTest :public LuaInterface
//{
//	// Í¨¹ý LuaInterface ¼Ì³Ð
//public:
//	virtual const char * GetNativeTypeName() override
//	{
//		return "LuaTest";
//	}
//	virtual const char* GetRequireScript()
//	{
//		return "test";
//	};
//public:
//	int intData;
//	void Say(const char* msg)
//	{
//		log_info("luat test say: %s", msg);
//	}
//	const char* getClassName()
//	{
//		return "Cpp.LuaTest";
//	}
//	int testData;
//	void SetTestData(int t)
//	{
//		testData = t;
//	}
//	int GetTestData() const
//	{
//		return testData;
//	}
//};
//
//int RegLua(lua_State* L)
//{
//	luabridge::getGlobalNamespace(L)
//		.beginClass<LuaTest>("LuaTest")
//		.addData("intData", &LuaTest::intData)
//		.addFunction("GetClassName", &LuaTest::getClassName)
//		.addProperty("testData", &LuaTest::GetTestData, &LuaTest::SetTestData)
//		.endClass();
//	return 0;
//}
//void test()
//{
//	LuaEngine::GetInstance()->RegisterModel(RegLua);
//	LuaEngine::GetInstance()->Start();
//	LuaTest t;
//	CallLua<LuaTest*>(&t);
//}

/////////////////////////////////////////////////////////////////////////////////
//script
//LuaTestExt = {}
//LuaTestExt.__index = LuaTestExt
//
//function LuaTestExt : OnEnter(config)
//print('lua enter....')
//print(config : GetClassName())
//config.intData = 100
//config.testData = 1234
//print(config.testData)
//end
