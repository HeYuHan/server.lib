#ifndef __SCRIPTBIND_LUA_LUAOBJECT_H__
#define __SCRIPTBIND_LUA_LUAOBJECT_H__

extern "C"
{
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

namespace Core
{
	class LuaRegister
	{
	public:
		virtual int OnRegister(lua_State *L) { return 0; }
	};

	struct LuaObject
	{
		LuaObject();
		LuaObject(LuaObject const& rhs);
		LuaObject& operator=(const LuaObject& rhs);
		LuaObject(lua_State* L, int index);
		LuaObject(lua_State* L, const char* name);

		virtual ~LuaObject();

		void Release();
		bool IsValid() const;
		void PushLuaObject(lua_State* L) const;

		void Init(lua_State* L, int index=-1);

		int				lua_ref_;
		lua_State*		lua_state_;
	};
	int _lua_call(lua_State* L, int params_num, int result_count, int n);
}

#endif // __SCRIPTBIND_LUA_LUAOBJECT_H__