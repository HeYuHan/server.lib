#pragma once
#ifndef __ILUA_SCRIPT_H__
#define __ILUA_SCRIPT_H__

extern "C"
{
#include "lua/lua.h"
#include "lua/lualib.h" 
#include "lua/lauxlib.h" 
}
#include "ScriptBind/Lua/LuaTable.h"
#include "ScriptBind/Lua/LuaFunction.h"
#include "ScriptBind/ExportHelp.h"
#include "ScriptBind/Script.h"
#include "ScriptBind/Lua/LuaBind.h"
#include "common.h"
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
	template<typename T>
	void EnterScript(T native);
private:
	//int m_LuaRef;
	LuaFunction m_EnterFunction;
	LuaTable m_Table;
	lua_State *L;
};
template<typename T>
void LuaInterface::EnterScript(T native)
{
	m_EnterFunction(m_Table, native);
}

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
	void LuaSearchPath(char *name, char *value);

	static LuaEngine* GetInstance();
private:
	void Pop(int old_top,const char* msg=0);
private:
	lua_State* L;
	static LuaEngine* m_Instance;
};
END_NS_CORE
#endif // !__ILUA_SCRIPT_H__



