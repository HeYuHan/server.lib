#include "LuaEngine.h"
#include <vector>
#include "log.h"
#include <ScriptBind/Lua/LuaBind.h>
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
	L = OpenLua();
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
	if (lua->m_Table.IsValid())
	{
		log_error("handle is create %d", lua->m_Table.lua_ref_);
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

	lua->m_Table = LuaTable(L,-1);
	
	if (lua->m_Table.IsValid())
	{
		lua->L = L;
		lua->m_EnterFunction = lua->m_Table.Get<LuaFunction>("OnEnter");
		if (!lua->m_EnterFunction.IsValid())
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
	if (lua->m_Table.IsValid())
	{
		lua->m_Table.Release();
	}
}

void LuaEngine::LuaSearchPath(char * name, char * value)
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

LuaInterface::LuaInterface():L(NULL)
{
}

LuaInterface::~LuaInterface()
{
	L = NULL;
}
END_NS_CORE