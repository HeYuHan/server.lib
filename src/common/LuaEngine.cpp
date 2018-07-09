#include "LuaEngine.h"
#include <vector>
#include <tolua++.h>
#include "log.h"

using namespace std;
vector<register_lua_model> all_model;
LuaEngine *LuaEngine::m_Instance = NULL;

LuaEngine::~LuaEngine()
{
	lua_close(L);
}

void LuaEngine::RegisterModel(register_lua_model model)
{
	all_model.push_back(model);
}

void LuaEngine::Start(const char* mainLua)
{
	L = lua_open();
	luaL_openlibs(L);
	
	for (int i=0;i<all_model.size();i++)
	{
		all_model[i](L);
	}
	if (mainLua)DoFile(mainLua);
}
bool LuaEngine::DoFile(const char* file_path)
{
	int top = lua_gettop(L);
	int error = luaL_dofile(L, file_path);
	if (error)
	{
		log_error("%s", lua_tostring(L, -1));
	}
	Pop(top, file_path);
	return error == 0;
}
bool LuaEngine::CreateScriptHandle(LuaInterface *lua)
{
	if (lua->m_LuaRef > 0)
	{
		log_error("handle is create %d", lua->m_LuaRef);
		return false;
	}
	lua->m_LuaRef = 0;
	int top = lua_gettop(L);
	char script[256] = { 0 };
	const char* extendName = lua->GetScriptTypeName();
	const char* require = lua->GetRequireScript();
	if (require == NULL)
	{
		sprintf(script, "return _G.%s%s",extendName ? extendName : lua->GetNativeTypeName(), extendName ? "" : "Ext");
	}
	else
	{
		sprintf(script, "require('%s')\r\nreturn _G.%s%s", require, extendName?extendName: lua->GetNativeTypeName(), extendName ? "" : "Ext");
	}
	std::string s(script);
	if (luaL_dostring(L, script))
	{
		log_error("%s", lua_tostring(L, -1));
		Pop(top);
		return false;
	}

	if (!lua_istable(L, -1))
	{
		log_error("NotFindScriptType:%s%s", extendName ? extendName : lua->GetNativeTypeName(), extendName ? "" : "Ext");
		Pop(top);
		return false;
	}
	lua_newtable(L);
	lua_newtable(L);
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -4);
	lua_settable(L, -3);

	/*lua_pushvalue(L, -3);
	lua_setfield(L, -1, "__index");*/
	lua_setmetatable(L, -2);



	lua_pushstring(L, "native");
	tolua_pushusertype(L, (void*)lua, lua->GetNativeTypeName());
	lua_pushstring(L, "handle");
	lua_pushvalue(L, -3);
	lua_settable(L, -3);
	lua_settable(L, -3);

	if (!lua_istable(L, -1))

	{
		log_error("%s not in top", lua->GetNativeTypeName());
		Pop(top);
		return false;
	}
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);
	Pop(top);
	lua->m_LuaRef = ref;
	lua->L = L;
	lua->EnterScript();
	
	return true;
}

void LuaEngine::DestoryScriptHandle(LuaInterface * lua)
{
	if (lua->m_LuaRef > 0)
	{
		luaL_unref(L, LUA_REGISTRYINDEX,lua->m_LuaRef);
		lua->m_LuaRef = 0;
	}
}

void LuaEngine::LuaSearchPath(char * name, char * value)
{
	std::string v;
	lua_getglobal(L, "package");
	lua_getfield(L, -1, name);
	v.append(lua_tostring(L, -1));
	v.append(";");
	v.append(value);
	lua_pushstring(L, v.c_str());
	lua_setfield(L, -3, name);
	lua_pop(L, 2);
}

LuaEngine * LuaEngine::GetInstance()
{
	if (m_Instance == NULL)m_Instance = new LuaEngine();
	return m_Instance;
}

void LuaEngine::Pop(int old_top,const char* msg)
{
	int top = lua_gettop(L);
	int ret = top - old_top;
	if (ret == 0)return;
	lua_settop(L,old_top);
#if _DEBUG
	log_info("%s pop value count:%d", msg, ret);
#endif
}

LuaInterface::LuaInterface():m_LuaRef(0),L(NULL)
{
}

LuaInterface::~LuaInterface()
{
	L = NULL;
}

bool LuaInterface::EnterScript()
{
	if (m_LuaRef == 0||NULL == L)return false;
	int top = lua_gettop(L);
	bool ok = true;
	lua_rawgeti(L, LUA_REGISTRYINDEX, m_LuaRef);
	ok = lua_istable(L, -1);
	if (!ok)
	{
		log_error("%s", "Native Script Ref Is Not Table");
		goto ret;
	}
	lua_getfield(L, -1, "OnEnter");
	ok = lua_isfunction(L, -1);
	if (!ok)
	{
		log_error("%s", "Script Eneter Function Not Found");
		goto ret;
	}
	lua_pushvalue(L,-2);
	int ret = lua_pcall(L, 1, 0, 0);
	if (ret)
	{
		log_error("%s", lua_tostring(L, -1));
		ret = false;
	}
ret:
	LuaEngine::GetInstance()->Pop(top);
	//lua_pop(L, 1);
	return ok;
}
