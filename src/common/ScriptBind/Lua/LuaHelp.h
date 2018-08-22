#ifndef __SCRIPTBIND_LUA_LUAHELP_H__
#define __SCRIPTBIND_LUA_LUAHELP_H__

extern "C"
{
#include "../../lua/lua.h"
#include "../../lua/lualib.h"
#include "../../lua/lauxlib.h"
};
#include <string>
#include "../../log.h"
#include "template.h"
namespace Core
{

#define LUA_ERROR(L, MSG, ...) { log_error(MSG, ##__VA_ARGS__); lua_pushstring(L, MSG); lua_error(L); }

#define ReturnValueFromLua() \
	R result = Pop<R>(lua_state_, -1); \
	lua_pop(lua_state_, 1); \
	return result; 
	int _lua_call(lua_State* L, int params_num, int result_count, int n);

	int lua_setuserdata_metatable();

	
	template<typename T> inline void Push(lua_State* L, T ret) { CppClass2Lua<T>::Write(L, ret);};
	template<> inline void Push(lua_State* L, char ret) { lua_pushnumber(L, ret);};
	template<> inline void Push(lua_State* L, unsigned char ret) { lua_pushnumber(L, ret);};
	template<> inline void Push(lua_State* L, short ret) { lua_pushnumber(L, ret);};
	template<> inline void Push(lua_State* L, unsigned short ret) { lua_pushnumber(L, ret);};
	template<> inline void Push(lua_State* L, long ret) { lua_pushnumber(L, ret);};
	template<> inline void Push(lua_State* L, unsigned long ret) { lua_pushnumber(L, ret);};
	template<> inline void Push(lua_State* L, int ret) { lua_pushnumber(L, ret);};
	template<> inline void Push(lua_State* L, unsigned int ret) { lua_pushnumber(L, ret);};
	template<> inline void Push(lua_State* L, float ret) { lua_pushnumber(L, ret);};
	template<> inline void Push(lua_State* L, double ret) { lua_pushnumber(L, ret);};
	template<> inline void Push(lua_State* L, char* ret) { lua_pushstring(L, ret);};
	template<> inline void Push(lua_State* L, std::string ret) { lua_pushstring(L, ret.c_str());};
	template<> inline void Push(lua_State* L, const char* ret) { lua_pushstring(L, ret);};
	template<> inline void Push(lua_State* L, const std::string& ret) { lua_pushstring(L, ret.c_str());};
	template<> inline void Push(lua_State* L, bool ret) { lua_pushboolean(L, ret);};
	template<> inline void Push(lua_State* L, long long ret) { return lua_pushnumber(L, (LUA_NUMBER)ret);};
	template<> inline void Push(lua_State* L, unsigned long long ret) { return lua_pushnumber(L, (LUA_NUMBER)ret);};

	//////////////////////////////////////////////////////////////////////////

	template<typename T> inline T			Pop(lua_State* L, int index) 
	{
		if(!lua_isuserdata(L,index)) LUA_ERROR(L, "this arg is not a userdata! T is %s\n", typeid(T).name());

		return Lua2CppClass<T>::Read(L, index);
	};
	template<>	inline void					Pop(lua_State* L, int index) {};
	template<>	inline bool					Pop(lua_State* L, int index) { return lua_toboolean(L, -1) ? true : false;};
	template<>	inline char*				Pop(lua_State* L, int index) { return (char*)lua_tostring(L, index); };
	template<>	inline const char*			Pop(lua_State* L, int index) { return (const char*)lua_tostring(L, index);};
	template<>	inline std::string			Pop(lua_State* L, int index) { return (const char*)lua_tostring(L, index);};
	template<>	inline const std::string&	Pop(lua_State* L, int index) { static std::string s; return s;};
	template<>	inline char					Pop(lua_State* L, int index) { return (char)lua_tonumber(L, index);};
	template<>	inline unsigned char		Pop(lua_State* L, int index) { return (unsigned char)lua_tonumber(L, index);};
	template<>	inline short				Pop(lua_State* L, int index) { return (short)lua_tonumber(L, index);};
	template<>	inline unsigned short		Pop(lua_State* L, int index) { return (unsigned short)lua_tonumber(L, index);};
	template<>	inline long					Pop(lua_State* L, int index) { return (long)lua_tonumber(L, index);};
	template<>	inline unsigned long		Pop(lua_State* L, int index) { return (unsigned long)lua_tonumber(L, index);};
	template<>	inline int					Pop(lua_State* L, int index) { return (int)lua_tonumber(L, index);};
	template<>	inline unsigned int			Pop(lua_State* L, int index) { return (unsigned int)lua_tonumber(L, index);};
	template<>	inline long long			Pop(lua_State* L, int index) { return (long long)lua_tonumber(L, index);};
	template<>  inline unsigned long long	Pop(lua_State* L, int index) { return (unsigned long long)lua_tonumber(L, index);};
	template<>	inline float				Pop(lua_State* L, int index) { return (float)lua_tonumber(L, index);};
	template<>	inline double				Pop(lua_State* L, int index) { return (double)lua_tonumber(L, index);};

	//////////////////////////////////////////////////////////////////////////
	

} // namespace Core

#endif // __SCRIPTBIND_LUA_LUAHELP_H__
