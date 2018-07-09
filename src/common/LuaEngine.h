#pragma once
#ifndef __ILUA_SCRIPT_H__
#define __ILUA_SCRIPT_H__

extern "C"
{
#include <lua.h>  
#include <lualib.h>  
#include <lauxlib.h>  
}

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
	bool EnterScript();
private:
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
	void LuaSearchPath(char *name, char *value);

	static LuaEngine* GetInstance();
private:
	void Pop(int old_top,const char* msg=0);
private:
	lua_State* L;
	static LuaEngine* m_Instance;
};

#endif // !__ILUA_SCRIPT_H__



