#ifndef __SCRIPTBIND_LUA_LUAFUNCTION_H__
#define __SCRIPTBIND_LUA_LUAFUNCTION_H__

#include "LuaObject.h"
#include "LuaBridge.h"
namespace Core
{

	struct LuaFunction : public LuaObject
	{
	public:
		LuaFunction() : LuaObject() {}
		LuaFunction(lua_State* L, int index) : LuaObject(L, index) {}
		LuaFunction(lua_State* L, const char* name) : LuaObject(L, name) {}

		void operator()(LuaObject* sender=NULL) 
		{
			int top = lua_gettop(lua_state_);
			if (sender)
			{
				sender->PushLuaObject(lua_state_);
			}
			lua_rawgeti(lua_state_, LUA_REGISTRYINDEX, lua_ref_);
			if (sender)
			{
				lua_pushvalue(lua_state_, -2);
			}
			_lua_call(lua_state_, 0 + (sender ? 1 : 0), 1, 0);
			lua_settop(lua_state_, top);
		}

		template<typename P1>
		void operator()(P1 p1, LuaObject* sender = NULL)
		{
			int top = lua_gettop(lua_state_);
			if (sender)
			{
				sender->PushLuaObject(lua_state_);
			}
			lua_rawgeti(lua_state_, LUA_REGISTRYINDEX, lua_ref_);
			if (sender)
			{
				lua_pushvalue(lua_state_, -2);
			}
			luabridge::push(lua_state_, p1);
			_lua_call(lua_state_, 1 + (sender ? 1 : 0), 1, 0);
			lua_settop(lua_state_, top);
		}

		template<typename P1, typename P2>
		void operator()(P1 p1, P2 p2, LuaObject* sender = NULL)
		{
			int top = lua_gettop(lua_state_);
			if (sender)
			{
				sender->PushLuaObject(lua_state_);
			}
			lua_rawgeti(lua_state_, LUA_REGISTRYINDEX, lua_ref_);
			if (sender)
			{
				lua_pushvalue(lua_state_, -2);
			}
			luabridge::push(lua_state_, p1);
			luabridge::push(lua_state_, p2);
			_lua_call(lua_state_, 2 + (sender ? 1 : 0), 1, 0);
			lua_settop(lua_state_, top);
		}

		template<typename P1, typename P2, typename P3>
		void operator()(P1 p1, P2 p2, P3 p3, LuaObject* sender = NULL)
		{
			int top = lua_gettop(lua_state_);
			if (sender)
			{
				sender->PushLuaObject(lua_state_);
			}
			lua_rawgeti(lua_state_, LUA_REGISTRYINDEX, lua_ref_);
			if (sender)
			{
				lua_pushvalue(lua_state_, -2);
			}
			luabridge::push(lua_state_, p1); 
			luabridge::push(lua_state_, p2); 
			luabridge::push(lua_state_, p3);
			_lua_call(lua_state_, 3 + (sender ? 1 : 0), 1, 0);
			lua_settop(lua_state_, top);
		}

		template<typename P1, typename P2, typename P3, typename P4>
		void operator()(P1 p1, P2 p2, P3 p3, P4 p4, LuaObject* sender = NULL)
		{
			int top = lua_gettop(lua_state_);
			if (sender)
			{
				sender->PushLuaObject(lua_state_);
			}
			lua_rawgeti(lua_state_, LUA_REGISTRYINDEX, lua_ref_);
			if (sender)
			{
				lua_pushvalue(lua_state_, -2);
			}
			luabridge::push(lua_state_, p1);
			luabridge::push(lua_state_, p2);
			luabridge::push(lua_state_, p3);
			luabridge::push(lua_state_, p4);
			_lua_call(lua_state_, 4 + (sender ? 1 : 0), 1, 0);
			lua_settop(lua_state_, top);
		}

		template<typename P1, typename P2, typename P3, typename P4, typename P5>
		void operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, LuaObject* sender = NULL)
		{
			int top = lua_gettop(lua_state_);
			if (sender)
			{
				sender->PushLuaObject(lua_state_);
			}
			lua_rawgeti(lua_state_, LUA_REGISTRYINDEX, lua_ref_);
			if (sender)
			{
				lua_pushvalue(lua_state_, -2);
			}
			luabridge::push(lua_state_, p1);
			luabridge::push(lua_state_, p2);
			luabridge::push(lua_state_, p3);
			luabridge::push(lua_state_, p4);
			luabridge::push(lua_state_, p5);
			_lua_call(lua_state_, 5 + (sender ? 1 : 0), 1, 0);
			lua_settop(lua_state_, top);
		}

		template<typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		void operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, LuaObject* sender = NULL)
		{
			int top = lua_gettop(lua_state_);
			if (sender)
			{
				sender->PushLuaObject(lua_state_);
			}
			lua_rawgeti(lua_state_, LUA_REGISTRYINDEX, lua_ref_);
			if (sender)
			{
				lua_pushvalue(lua_state_, -2);
			}
			luabridge::push(lua_state_, p1);
			luabridge::push(lua_state_, p2);
			luabridge::push(lua_state_, p3); 
			luabridge::push(lua_state_, p4);
			luabridge::push(lua_state_, p5);
			luabridge::push(lua_state_, p6);
			_lua_call(lua_state_, 6 + (sender ? 1 : 0), 1, 0);
			lua_settop(lua_state_, top);
		}

		template<typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		void operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, LuaObject* sender = NULL)
		{
			int top = lua_gettop(lua_state_);
			if (sender)
			{
				sender->PushLuaObject(lua_state_);
			}
			lua_rawgeti(lua_state_, LUA_REGISTRYINDEX, lua_ref_);
			if (sender)
			{
				lua_pushvalue(lua_state_, -2);
			}
			luabridge::push(lua_state_, p1);
			luabridge::push(lua_state_, p2);
			luabridge::push(lua_state_, p3);
			luabridge::push(lua_state_, p4); 
			luabridge::push(lua_state_, p5);
			luabridge::push(lua_state_, p6);
			luabridge::push(lua_state_, p7);
			_lua_call(lua_state_, 7 + (sender ? 1 : 0), 1, 0);
			lua_settop(lua_state_, top);
		}

		template<typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		void operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, LuaObject* sender = NULL)
		{
			int top = lua_gettop(lua_state_);
			if (sender)
			{
				sender->PushLuaObject(lua_state_);
			}
			lua_rawgeti(lua_state_, LUA_REGISTRYINDEX, lua_ref_);
			if (sender)
			{
				lua_pushvalue(lua_state_, -2);
			}
			luabridge::push(lua_state_, p1);
			luabridge::push(lua_state_, p2); 
			luabridge::push(lua_state_, p3);
			luabridge::push(lua_state_, p4); 
			luabridge::push(lua_state_, p5); 
			luabridge::push(lua_state_, p6);
			luabridge::push(lua_state_, p7);
			luabridge::push(lua_state_, p8);
			_lua_call(lua_state_, 8 + (sender ? 1 : 0), 1, 0);
			lua_settop(lua_state_, top);
		}

		template<typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		void operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, LuaObject* sender = NULL)
		{
			int top = lua_gettop(lua_state_);
			if (sender)
			{
				sender->PushLuaObject(lua_state_);
			}
			lua_rawgeti(lua_state_, LUA_REGISTRYINDEX, lua_ref_);
			if (sender)
			{
				lua_pushvalue(lua_state_, -2);
			}
			luabridge::push(lua_state_, p1);
			luabridge::push(lua_state_, p2);
			luabridge::push(lua_state_, p3);
			luabridge::push(lua_state_, p4);
			luabridge::push(lua_state_, p5);
			luabridge::push(lua_state_, p6);
			luabridge::push(lua_state_, p7);
			luabridge::push(lua_state_, p8); 
			luabridge::push(lua_state_, p9);
			_lua_call(lua_state_, 9 + (sender ? 1 : 0), 1, 0);
			lua_settop(lua_state_, top);
		}
	};
}

#endif // __SCRIPTBIND_LUA_LUAFUNCTION_H__