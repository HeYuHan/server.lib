#include <stdio.h>
#include <Windows.h>
extern "C"
{
#include <lua.h>  
#include <lualib.h>  
#include <lauxlib.h>  
}
#include "lua_mytestclass.hpp"
#include "Student.h"
#include <LuaEngine.h>

int main(int argc, char **args)
{
	//lua_State *L = lua_open();
	//luaL_openlibs(L);
	//int top = lua_gettop(L);
	//register_all_lua_mytestclass(L);
	//lua_State *L2 = lua_newthread(L);
	//lua_pushthread(L2);
	//lua_newtable(L2);
	//lua_newtable(L2);
	//lua_getglobal(L2, "_G");
	//lua_setfield(L2, -2, "__index");
	///*lua_pushfstring(L2, "__index");
	//bool _G = lua_istable(L2, -4);
	//lua_pushvalue(L2, -4);
	//lua_settable(L2, -3);*/
	//lua_setmetatable(L2, -2);
	//if (!lua_setfenv(L2, -2))
	//{
	//	printf_s("error:%s", "setfenv fail!\n");
	//}
	////luaL_dostring(L2,"pirnt(1234)");

	//char *buff = "print(123)";
	//luaL_loadbuffer(L2, buff, strlen(buff), "line");
	//
	//if (lua_pcall(L2, 0, 0, 0))
	//{
	//	printf_s("error:%s", lua_tostring(L2, -1));
	//}
	//lua_pop(L2,1);


	//
	////lua_pushfstring(L, "testkey");
	////lua_pushnumber(L, 1211);

	////lua_settable(L, -3);
	////
	////lua_pushvalue(L, -1);
	////int ref = luaL_ref(L, LUA_REGISTRYINDEX);
	////lua_newtable(L);
	////lua_newtable(L);
	////lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
	////lua_getfield(L, -1, "testkey");
	////int tt = lua_tonumber(L,-1);
	////lua_pop(L, 1);

	//Student s;
	//lua_getglobal(L,"newfenv");
	//lua_getfield(L, -1, "OnEnter");
	//tolua_pushusertype(L, (void*)&s, "Student");
	//lua_pcall(L, 1, 0, 0);
	//lua_pop(L, 1);
	//lua_close(L);
	//system("pause");
	//return 0;

	LuaEngine::GetInstance()->RegisterModel(register_all_lua_mytestclass);
	LuaEngine::GetInstance()->Start("script/main.lua");
	Student s;
	s.t = 10;
	LuaEngine::GetInstance()->CreateScriptHandle(&s);
	s.t = 20;
	s.EnterScript();
	Student s2;
	s2.t = 30;
	LuaEngine::GetInstance()->CreateScriptHandle(&s2);
	system("pause");
	return 0;
}