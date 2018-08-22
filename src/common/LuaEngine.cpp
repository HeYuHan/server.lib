#include "LuaEngine.h"
#include <vector>
#include "log.h"
using namespace std;
BEGIN_NS_CORE
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
	L = luaL_newstate();
	luaL_openlibs(L);
	
	for (size_t i=0;i<all_model.size();i++)
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
	if (lua->m_LuaRef>0)
	{
		log_error("handle is create %d", lua->m_LuaRef);
		return false;
	}
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
	
	lua_setmetatable(L, -2);
	
	if (lua_istable(L,-1))
	{
		lua->L = L;
		lua_getfield(L, -1, "OnEnter");
		if (lua_isfunction(L, -1))
		{
			lua->m_LuaRef = luaL_ref(L, LUA_REGISTRYINDEX);
			lua_pop(L, 1);
		}
		else
		{
			log_error("not found enter function %p", lua);
			Pop(top);
			return false;
		}
	}
	else
	{
		log_error("%s not in top", lua->GetNativeTypeName());
		Pop(top);
		return false;
	}
	Pop(top);
	
	return true;
}

void LuaEngine::DestoryScriptHandle(LuaInterface * lua)
{
	if (lua->m_LuaRef>0)
	{
		luaL_unref(lua->L, LUA_REGISTRYINDEX, lua->m_LuaRef);
		lua->m_LuaRef = -1;
	}
}

void LuaEngine::LuaSearchPath(const char * name, const char * value)
{
	std::string v;
	int top = lua_gettop(L);
	lua_getglobal(L, "package");
	lua_getfield(L, -1, name);
	if (lua_isnil(L, -1))
	{
		log_error("cant get search path:%s", name);
		goto exit;
	}
	v.append(lua_tostring(L, -1));
	v.append(";");
	v.append(value);
	lua_pushstring(L, v.c_str());
	lua_setfield(L, -3, name);
	exit:
	Pop(top);
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
	log_info("%s pop value count:%d", msg?msg:"lua", ret);
#endif
}

LuaInterface::LuaInterface():L(NULL), m_LuaRef(-1)
{
	
}

LuaInterface::~LuaInterface()
{
	LuaEngine::GetInstance()->DestoryScriptHandle(this);
	L = NULL;
}
END_NS_CORE