#include "Script.h"

#include "ExportHelp.h"

namespace Core
{

	void run_buildin_bindings()
	{
		RegisterGlobalFunction(GetLuaState(), "lua_setuserdata_metatable", lua_setuserdata_metatable);
	}
	

} // namespace Core

