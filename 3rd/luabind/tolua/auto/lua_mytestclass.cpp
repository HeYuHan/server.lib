#include "lua_mytestclass.hpp"
#include "Student.h"
#include "tolua_fix.h"
#include "LuaBasicConversions.h"



int lua_lua_mytestclass_Student_Run(lua_State* tolua_S)
{
    int argc = 0;
    Student* cobj = nullptr;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"Student",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (Student*)tolua_tousertype(tolua_S,1,0);
#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_lua_mytestclass_Student_Run'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            int arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "Student:Run");

            if (!ok) { break; }
            cobj->Run(arg0);
            return 0;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 0) {
            cobj->Run();
            return 0;
        }
    }while(0);
    ok  = true;
    CCLOG("%s has wrong number of arguments: %d, was expecting %d \n",  "Student:Run",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_lua_mytestclass_Student_Run'.",&tolua_err);
#endif

    return 0;
}
int lua_lua_mytestclass_Student_RunScript(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"Student",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        Student* arg0;
        ok &= luaval_to_object<Student>(tolua_S, 2, "Student",&arg0);
        if(!ok)
            return 0;
        Student::RunScript(arg0);
        return 0;
    }
    CCLOG("%s has wrong number of arguments: %d, was expecting %d\n ", "Student:RunScript",argc, 1);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_lua_mytestclass_Student_RunScript'.",&tolua_err);
#endif
    return 0;
}
int lua_lua_mytestclass_Student_constructor(lua_State* tolua_S)
{
    int argc = 0;
    Student* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
            return 0;
        cobj = new Student();
        tolua_pushusertype(tolua_S,(void*)cobj,"Student");
        tolua_register_gc(tolua_S,lua_gettop(tolua_S));
        return 1;
    }
    CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "Student:Student",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_lua_mytestclass_Student_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_lua_mytestclass_Student_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Student)");
    return 0;
}

int lua_register_lua_mytestclass_Student(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"Student");
    tolua_cclass(tolua_S,"Student","Student","",nullptr);

    tolua_beginmodule(tolua_S,"Student");
        tolua_function(tolua_S,"new",lua_lua_mytestclass_Student_constructor);
        tolua_function(tolua_S,"Run",lua_lua_mytestclass_Student_Run);
        tolua_function(tolua_S,"RunScript", lua_lua_mytestclass_Student_RunScript);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(Student).name();
    g_luaType[typeName] = "Student";
    g_typeCast["Student"] = "Student";
    return 1;
}
TOLUA_API int register_all_lua_mytestclass(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	
	tolua_module(tolua_S,nullptr,0);
	tolua_beginmodule(tolua_S,nullptr);

	lua_register_lua_mytestclass_Student(tolua_S);

	tolua_endmodule(tolua_S);
	return 1;
}

