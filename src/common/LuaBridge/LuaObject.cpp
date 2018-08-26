#include "LuaObject.h"
#include <log.h>
namespace Core
{

	LuaObject::LuaObject() : lua_ref_(LUA_REFNIL), lua_state_(NULL)
	{

	}

	LuaObject::LuaObject( LuaObject const& rhs )
	{
		if(rhs.IsValid()) 
		{
			lua_rawgeti(rhs.lua_state_, LUA_REGISTRYINDEX, rhs.lua_ref_);
			Init(rhs.lua_state_, -1);
		}
		else
		{
			lua_ref_ = LUA_REFNIL;
		}
	}

	LuaObject::LuaObject( lua_State* L, int index )
	{
		int oldTop=lua_gettop(L);
		Init(L, index);
		lua_settop(L,oldTop);
	}

	LuaObject::LuaObject( lua_State* L, const char* name )
	{
		int oldTop=lua_gettop(L);
		lua_getglobal(L, name);
		Init(L, -1);
		lua_settop(L,oldTop);
	}

	LuaObject& LuaObject::operator=( const LuaObject& rhs )
	{
		if(rhs.IsValid()) 
		{
			lua_rawgeti(rhs.lua_state_, LUA_REGISTRYINDEX, rhs.lua_ref_);
			Init(rhs.lua_state_, -1);
		}
		else
		{
			lua_ref_ = LUA_REFNIL;
		}
		return *this;
	}

	LuaObject::~LuaObject()
	{
		Release();
	}

	void LuaObject::Release()
	{
		if (IsValid()) 
			luaL_unref(lua_state_, LUA_REGISTRYINDEX, lua_ref_);
		lua_ref_ = 0;
		lua_state_ = NULL;
	}

	bool LuaObject::IsValid() const
	{
		return lua_ref_ != LUA_REFNIL;
	}

	void LuaObject::Init( lua_State* L, int index )
	{
		Release();
		if (lua_istable(L, index) || lua_isfunction(L, index)) 
		{
			lua_ref_ = luaL_ref(L, LUA_REGISTRYINDEX);
		}
		else 
		{
			lua_ref_ = LUA_REFNIL;
			log_error("%s","can't not find lua object\n");
		}
		lua_state_ = L;
	}

	void LuaObject::PushLuaObject( lua_State* L ) const
	{
		if(IsValid()) 
			lua_rawgeti(L, LUA_REGISTRYINDEX, lua_ref_);
		else 
			lua_pushnil(L);
	}
#ifdef _DEBUG
	static const char* _lua_pop_string(lua_State* L)
	{
		lua_pop(L, 1);
		const char* str = lua_tostring(L, 0);
		return str;
}

	static int _lua_handle_error(lua_State* L)
	{
		lua_getglobal(L, "debug");
		if (!lua_istable(L, -1))
		{
			lua_pop(L, 1);
			return 1;
		}

		lua_getfield(L, -1, "traceback");
		if (!lua_isfunction(L, -1))
		{
			lua_pop(L, 2);
			return 1;
		}

		lua_pushvalue(L, 1);
		lua_pushinteger(L, 2);
		lua_call(L, 2, 1);
		return 1;
	}
#endif // _DEBUG

	int _lua_call(lua_State* L, int params_num, int result_count, int n)
	{
#ifndef _DEBUG
		return lua_pcall(L, params_num, result_count, n);

#else
		int size = lua_gettop(L);
		int handler = lua_gettop(L) - params_num;
		lua_pushcfunction(L, _lua_handle_error);
		lua_insert(L, handler);

		int ret = 0;
		if (lua_pcall(L, params_num, result_count, handler))
		{
			ret = LUA_ERRRUN;
			log_error("%s", _lua_pop_string(L));
		}
		lua_remove(L, handler);

		if (((lua_gettop(L) + (int)params_num + 1 - result_count) != size))
		{
			log_error("%s", "Lua error: lua stack size changed!");
		}

		return ret;
#endif
	}

}

