/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "LuaBasicConversions.h"
#include "tolua_fix.h"
//#include "deprecated/CCBool.h"
//
//#include "deprecated/CCDouble.h"
//#include "deprecated/CCFloat.h"
//#include "deprecated/CCInteger.h"

std::unordered_map<std::string, std::string>  g_luaType;
std::unordered_map<std::string, std::string>  g_typeCast;

#if COCOS2D_DEBUG >=1
void luaval_to_native_err(lua_State* L,const char* msg,tolua_Error* err, const char* funcName)
{
    if (NULL == L || NULL == err || NULL == msg || 0 == strlen(msg))
        return;

    if (msg[0] == '#')
    {
        const char* expected = err->type;
        const char* provided = tolua_typename(L,err->index);
        if (msg[1]=='f')
        {
            int narg = err->index;
            if (err->array)
                CCLOG("%s\n     %s argument #%d is array of '%s'; array of '%s' expected.\n",msg+2,funcName,narg,provided,expected);
            else
                CCLOG("%s\n     %s argument #%d is '%s'; '%s' expected.\n",msg+2,funcName,narg,provided,expected);
        }
        else if (msg[1]=='v')
        {
            if (err->array)
                CCLOG("%s\n     %s value is array of '%s'; array of '%s' expected.\n",funcName,msg+2,provided,expected);
            else
                CCLOG("%s\n     %s value is '%s'; '%s' expected.\n",msg+2,funcName,provided,expected);
        }
    }
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
extern int lua_isusertype (lua_State* L, int lo, const char* type);
#ifdef __cplusplus
}
#endif

//bool luaval_is_usertype(lua_State* L,int lo,const char* type, int def)
//{
//    if (def && lua_gettop(L)<std::abs(lo))
//        return true;
//
//    if (lua_isnil(L,lo) || lua_isusertype(L,lo,type))
//        return true;
//
//    return false;
//}

bool luaval_to_ushort(lua_State* L, int lo, unsigned short* outValue, const char* funcName)
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L,lo,0,&tolua_err))
    {
#if COCOS2D_DEBUG >=1
        luaval_to_native_err(L,"#ferror:",&tolua_err,funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        *outValue = (unsigned short)tolua_tonumber(L, lo, 0);
    }

    return ok;
}


bool luaval_to_int32(lua_State* L,int lo,int* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L,lo,0,&tolua_err))
    {
#if COCOS2D_DEBUG >=1
        luaval_to_native_err(L,"#ferror:",&tolua_err,funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        /**
         When we want to convert the number value from the Lua to int, we would call lua_tonumber to implement.It would
         experience two phase conversion: int -> double, double->int.But,for the 0x80000000 which the min value of int, the
         int cast may return an undefined result,like 0x7fffffff.So we must use the (int)(unsigned int)lua_tonumber() to get
         predictable results for 0x80000000.In this place,we didn't use lua_tointeger, because it may produce different results
         depending on the compiler,e.g:for iPhone4s,it also get wrong value for 0x80000000.
         */
        unsigned int estimateValue = (unsigned int)lua_tonumber(L, lo);
        if (estimateValue == std::numeric_limits<int>::min())
        {
            *outValue = (int)estimateValue;
        }
        else
        {
            *outValue = (int)lua_tonumber(L, lo);
        }
    }

    return ok;
}

bool luaval_to_uint32(lua_State* L, int lo, unsigned int* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L,lo,0,&tolua_err))
    {
#if COCOS2D_DEBUG >=1
        luaval_to_native_err(L,"#ferror:",&tolua_err,funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        *outValue = (unsigned int)tolua_tonumber(L, lo, 0);
    }

    return ok;
}

bool luaval_to_uint16(lua_State* L,int lo,uint16_t* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L,lo,0,&tolua_err))
    {
#if COCOS2D_DEBUG >=1
        luaval_to_native_err(L,"#ferror:",&tolua_err,funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        *outValue = (unsigned char)tolua_tonumber(L, lo, 0);
    }

    return ok;
}

bool luaval_to_boolean(lua_State* L,int lo,bool* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isboolean(L,lo,0,&tolua_err))
    {
#if COCOS2D_DEBUG >=1
        luaval_to_native_err(L,"#ferror:",&tolua_err,funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        *outValue = (bool)tolua_toboolean(L, lo, 0);
    }

    return ok;
}

bool luaval_to_number(lua_State* L,int lo,double* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L,lo,0,&tolua_err))
    {
#if COCOS2D_DEBUG >=1
        luaval_to_native_err(L,"#ferror:",&tolua_err,funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        *outValue = tolua_tonumber(L, lo, 0);
    }

    return ok;
}

bool luaval_to_long_long(lua_State* L,int lo,long long* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L,lo,0,&tolua_err))
    {
#if COCOS2D_DEBUG >=1
        luaval_to_native_err(L,"#ferror:",&tolua_err,funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        *outValue = (long long)tolua_tonumber(L, lo, 0);
    }

    return ok;
}

bool luaval_to_std_string(lua_State* L, int lo, std::string* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_iscppstring(L,lo,0,&tolua_err))
    {
#if COCOS2D_DEBUG >=1
        luaval_to_native_err(L,"#ferror:",&tolua_err,funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        size_t size;
        auto rawString = lua_tolstring(L,lo,&size);
        *outValue = std::string(rawString, size);
    }

    return ok;
}


bool luaval_to_long(lua_State* L,int lo, long* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L,lo,0,&tolua_err))
    {
#if COCOS2D_DEBUG >=1
        luaval_to_native_err(L,"#ferror:",&tolua_err,funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        *outValue = (long)tolua_tonumber(L, lo, 0);
    }

    return ok;
}

bool luaval_to_ulong(lua_State* L,int lo, unsigned long* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L,lo,0,&tolua_err))
    {
#if COCOS2D_DEBUG >=1
        luaval_to_native_err(L,"#ferror:",&tolua_err,funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        *outValue = (unsigned long)tolua_tonumber(L, lo, 0);
    }

    return ok;
}



//bool luaval_to_array(lua_State* L,int lo, __Array** outValue, const char* funcName)
//{
//    if (NULL == L || NULL == outValue)
//        return false;
//
//    bool ok = true;
//
//    tolua_Error tolua_err;
//    if (!tolua_istable(L, lo, 0, &tolua_err) )
//    {
//#if COCOS2D_DEBUG >=1
//        luaval_to_native_err(L,"#ferror:",&tolua_err,funcName);
//#endif
//        ok = false;
//    }
//
//    if (ok)
//    {
//        size_t len = lua_objlen(L, lo);
//        if (len > 0)
//        {
//            __Array* arr =  __Array::createWithCapacity(len);
//            if (NULL == arr)
//                return false;
//
//            for (size_t i = 0; i < len; i++)
//            {
//                lua_pushnumber(L,i + 1);
//                lua_gettable(L,lo);
//                if (lua_isnil(L,-1))
//                {
//                    lua_pop(L, 1);
//                    continue;
//                }
//
//                if (lua_isuserdata(L, -1))
//                {
//                    Ref* obj = static_cast<Ref*>(tolua_tousertype(L, -1, NULL) );
//                    if (NULL != obj)
//                    {
//                        arr->addObject(obj);
//                    }
//                }
//                else if(lua_istable(L, -1))
//                {
//                    lua_pushnumber(L,1);
//                    lua_gettable(L,-2);
//                    if (lua_isnil(L, -1) )
//                    {
//                        lua_pop(L,1);
//                        __Dictionary* dictVal = NULL;
//                        if (luaval_to_dictionary(L,-1,&dictVal))
//                        {
//                            arr->addObject(dictVal);
//                        }
//                    }
//                    else
//                    {
//                       lua_pop(L,1);
//                       __Array* arrVal = NULL;
//                       if(luaval_to_array(L, -1, &arrVal))
//                       {
//                           arr->addObject(arrVal);
//                       }
//                    }
//                }
//                else if(lua_type(L, -1) == LUA_TSTRING)
//                {
//                    std::string stringValue = "";
//                    if(luaval_to_std_string(L, -1, &stringValue) )
//                    {
//                        arr->addObject(__String::create(stringValue));
//                    }
//                }
//                else if(lua_type(L, -1) == LUA_TBOOLEAN)
//                {
//                    bool boolVal = false;
//                    if (luaval_to_boolean(L, -1, &boolVal))
//                    {
//                        arr->addObject(__Bool::create(boolVal));
//                    }
//                }
//                else if(lua_type(L, -1) == LUA_TNUMBER)
//                {
//                    arr->addObject(__Double::create(tolua_tonumber(L, -1, 0)));
//                }
//                else
//                {
//                    CCASSERT(false, "not supported type");
//                }
//                lua_pop(L, 1);
//            }
//
//            *outValue = arr;
//        }
//    }
//
//    return ok;
//}
//
//bool luaval_to_dictionary(lua_State* L,int lo, __Dictionary** outValue, const char* funcName)
//{
//    if (NULL == L || NULL == outValue)
//        return  false;
//
//    bool ok = true;
//
//    tolua_Error tolua_err;
//    if (!tolua_istable(L, lo, 0, &tolua_err) )
//    {
//#if COCOS2D_DEBUG >=1
//        luaval_to_native_err(L,"#ferror:",&tolua_err,funcName);
//#endif
//        ok = false;
//    }
//
//    if (ok)
//    {
//        std::string stringKey = "";
//        std::string stringValue = "";
//        bool boolVal = false;
//        __Dictionary* dict = NULL;
//        lua_pushnil(L);                                             /* L: lotable ..... nil */
//        while ( 0 != lua_next(L, lo ) )                             /* L: lotable ..... key value */
//        {
//            if (!lua_isstring(L, -2))
//            {
//                lua_pop(L, 1);
//                continue;
//            }
//
//            if (NULL == dict)
//            {
//                dict = __Dictionary::create();
//            }
//
//            if(luaval_to_std_string(L, -2, &stringKey))
//            {
//                if (lua_isuserdata(L, -1))
//                {
//                    Ref* obj = static_cast<Ref*>(tolua_tousertype(L, -1, NULL) );
//                    if (NULL != obj)
//                    {
//                        //get the key to string
//                        dict->setObject(obj, stringKey);
//                    }
//                }
//                else if(lua_istable(L, -1))
//                {
//                    lua_pushnumber(L,1);
//                    lua_gettable(L,-2);
//                    if (lua_isnil(L, -1) )
//                    {
//                        lua_pop(L,1);
//                        __Dictionary* dictVal = NULL;
//                        if (luaval_to_dictionary(L,-1,&dictVal))
//                        {
//                            dict->setObject(dictVal,stringKey);
//                        }
//                    }
//                    else
//                    {
//                        lua_pop(L,1);
//                        __Array* arrVal = NULL;
//                        if(luaval_to_array(L, -1, &arrVal))
//                        {
//                            dict->setObject(arrVal,stringKey);
//                        }
//                    }
//                }
//                else if(lua_type(L, -1) == LUA_TSTRING)
//                {
//                    if(luaval_to_std_string(L, -1, &stringValue))
//                    {
//                        dict->setObject(__String::create(stringValue), stringKey);
//                    }
//                }
//                else if(lua_type(L, -1) == LUA_TBOOLEAN)
//                {
//                    if (luaval_to_boolean(L, -1, &boolVal))
//                    {
//                        dict->setObject(__Bool::create(boolVal),stringKey);
//                    }
//                }
//                else if(lua_type(L, -1) == LUA_TNUMBER)
//                {
//                     dict->setObject(__Double::create(tolua_tonumber(L, -1, 0)),stringKey);
//                }
//                else
//                {
//                    CCASSERT(false, "not supported type");
//                }
//            }
//
//            lua_pop(L, 1);                                          /* L: lotable ..... key */
//        }
//
//                                                                    /* L: lotable ..... */
//        *outValue = dict;
//    }
//
//    return ok;
//}



bool luaval_to_std_vector_string(lua_State* L, int lo, std::vector<std::string>* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret || lua_gettop(L) < lo)
        return false;

    tolua_Error tolua_err;
    bool ok = true;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if COCOS2D_DEBUG >=1
        luaval_to_native_err(L,"#ferror:",&tolua_err,funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        size_t len = lua_objlen(L, lo);
        std::string value = "";
        for (size_t i = 0; i < len; i++)
        {
            lua_pushnumber(L, i + 1);
            lua_gettable(L,lo);
            if(lua_isstring(L, -1))
            {
                ok = luaval_to_std_string(L, -1, &value);
                if(ok)
                    ret->push_back(value);
            }
            else
            {
                //CCASSERT(false, "string type is needed");
            }

            lua_pop(L, 1);
        }
    }

    return ok;
}

bool luaval_to_std_vector_int(lua_State* L, int lo, std::vector<int>* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret || lua_gettop(L) < lo)
        return false;

    tolua_Error tolua_err;
    bool ok = true;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if COCOS2D_DEBUG >=1
        luaval_to_native_err(L,"#ferror:",&tolua_err,funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        size_t len = lua_objlen(L, lo);
        for (size_t i = 0; i < len; i++)
        {
            lua_pushnumber(L, i + 1);
            lua_gettable(L,lo);
            if(lua_isnumber(L, -1))
            {
                ret->push_back((int)tolua_tonumber(L, -1, 0));
            }
            else
            {
                //CCASSERT(false, "int type is needed");
            }

            lua_pop(L, 1);
        }
    }

    return ok;
}



bool luaval_to_std_vector_float(lua_State* L, int lo, std::vector<float>* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret || lua_gettop(L) < lo)
        return false;

    tolua_Error tolua_err;
    bool ok = true;

    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if COCOS2D_DEBUG >=1
        luaval_to_native_err(L,"#ferror:",&tolua_err,funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        size_t len = lua_objlen(L, lo);
        for (size_t i = 0; i < len; i++)
        {
            lua_pushnumber(L, i + 1);
            lua_gettable(L,lo);
            if(lua_isnumber(L, -1))
            {
                ret->push_back((float)tolua_tonumber(L, -1, 0));
            }
            else
            {
                //CCASSERT(false, "float type is needed");
            }

            lua_pop(L, 1);
        }
    }

    return ok;
}


bool luaval_to_std_vector_ushort(lua_State* L, int lo, std::vector<unsigned short>* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret || lua_gettop(L) < lo)
        return false;

    tolua_Error tolua_err;
    bool ok = true;

    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if COCOS2D_DEBUG >=1
        luaval_to_native_err(L,"#ferror:",&tolua_err,funcName);
#endif
        ok = false;
    }

    if (ok)
    {
        size_t len = lua_objlen(L, lo);
        for (size_t i = 0; i < len; i++)
        {
            lua_pushnumber(L, i + 1);
            lua_gettable(L,lo);
            if(lua_isnumber(L, -1))
            {
                ret->push_back((unsigned short)tolua_tonumber(L, -1, 0));
            }
            else
            {
                //ASSERT(false, "unsigned short type is needed");
            }

            lua_pop(L, 1);
        }
    }

    return ok;
}




void ccvector_std_string_to_luaval(lua_State* L, const std::vector<std::string>& inValue)
{
    if (nullptr == L)
        return;

    lua_newtable(L);

    int index = 1;

    for (const std::string& value : inValue)
    {
        lua_pushnumber(L, (lua_Number)index);
        lua_pushstring(L, value.c_str());
        lua_rawset(L, -3);
        ++index;
    }
}

void ccvector_int_to_luaval(lua_State* L, const std::vector<int>& inValue)
{
    if (nullptr == L)
        return;

    lua_newtable(L);

    int index = 1;
    for (const int value : inValue)
    {
        lua_pushnumber(L, (lua_Number)index);
        lua_pushnumber(L, (lua_Number)value);
        lua_rawset(L, -3);
        ++index;
    }
}

void ccvector_float_to_luaval(lua_State* L, const std::vector<float>& inValue)
{
    if (nullptr == L)
        return;

    lua_newtable(L);

    int index = 1;
    for (const float value : inValue)
    {
        lua_pushnumber(L, (lua_Number)index);
        lua_pushnumber(L, (lua_Number)value);
        lua_rawset(L, -3);
        ++index;
    }
}

void ccvector_ushort_to_luaval(lua_State* L, const std::vector<unsigned short>& inValue)
{
    if (nullptr == L)
        return;

    lua_newtable(L);

    int index = 1;
    for (const unsigned short value : inValue)
    {
        lua_pushnumber(L, (lua_Number)index);
        lua_pushnumber(L, (lua_Number)value);
        lua_rawset(L, -3);
        ++index;
    }
}


//void std_map_string_string_to_luaval(lua_State* L, const std::map<std::string, std::string>& inValue)
//{
//    if (nullptr == L)
//        return;
//
//    lua_newtable(L);
//
//    for (auto iter = inValue.begin(); iter != inValue.end(); ++iter)
//    {
//        lua_pushstring(L, iter->first.c_str());
//        lua_pushstring(L, iter->second.c_str());
//        lua_rawset(L, -3);
//    }
//}
//
//bool luaval_to_std_map_string_string(lua_State* L, int lo, std::map<std::string, std::string>* ret, const char* funcName)
//{
//    if (nullptr == L || nullptr == ret || lua_gettop(L) < lo)
//        return false;
//
//    tolua_Error tolua_err;
//    bool ok = true;
//    if (!tolua_istable(L, lo, 0, &tolua_err))
//    {
//#if COCOS2D_DEBUG >=1
//        luaval_to_native_err(L,"#ferror:",&tolua_err,funcName);
//#endif
//        ok = false;
//    }
//
//    if (!ok)
//        return ok;
//
//    lua_pushnil(L);
//    std::string key;
//    std::string value;
//    while (lua_next(L, lo) != 0)
//    {
//        if (lua_isstring(L, -2) && lua_isstring(L, -1))
//        {
//            if (luaval_to_std_string(L, -2, &key) && luaval_to_std_string(L, -1, &value))
//            {
//                (*ret)[key] = value;
//            }
//        }
//        else
//        {
//            CCASSERT(false, "string type is needed");
//        }
//
//        lua_pop(L, 1);
//    }
//
//    return ok;
//}