#pragma once
#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__
#include <typeinfo>
struct ClassMetaData
{
public:
	ClassMetaData() : parent_(NULL), meta_table_ref_(LUA_REFNIL), lua_state_(NULL) {}
	virtual ~ClassMetaData() {}

	inline void ReleaseBeforeCloseLua()
	{
		if (IsValidRef()) luaL_unref(lua_state_, LUA_REGISTRYINDEX, meta_table_ref_); meta_table_ref_ = LUA_REFNIL;
	}
	inline void SetParent(ClassMetaData* parent) { parent_ = parent; }
	inline ClassMetaData* GetParent() { return parent_; }
	inline const char* GetName() { return name_.c_str(); }
	inline void SetName(const char* name) { name_ = name; }
	inline bool IsValidRef() { return meta_table_ref_ != LUA_REFNIL && meta_table_ref_ != LUA_NOREF; }
	inline void PushMetaTable(lua_State* L)
	{
		int ref = IsValidRef() ? meta_table_ref_ : LUA_REFNIL;
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
	}

	ClassMetaData*	parent_;
	int				meta_table_ref_;
	lua_State*		lua_state_;
	std::string		name_;

private:
	ClassMetaData(ClassMetaData const& rhs);
	ClassMetaData& operator=(const ClassMetaData& rhs);
};



template<typename T>
struct NullLifePolicy
{
	static inline void OnAllocated(T* /*self*/) {}
	static inline void OnGC(T* /*self*/) {}
	static inline void OnConstructor(T* /*self*/) {}
};

template<typename T>
struct UserData
{
	UserData(T* obj) : cpp_obj_(obj) {};
	~UserData() {};

	T* cpp_obj_;
};



template<typename T, typename P = NullLifePolicy<T> >
struct ScriptMetaClass
{
	typedef P LifePolicy;

	inline static const char* GetName() { return meta_data_.GetName(); }
	inline static void SetName(const char* name) { meta_data_.SetName(name); }
	inline static void SetParent(ClassMetaData* parent) { meta_data_.SetParent(parent); }
	inline static ClassMetaData* GetParent() { return meta_data_.GetParent(); }
	inline static bool IsBuild() { return meta_data_.IsValidRef(); }
	inline static ClassMetaData* GetClassMetaData() { return &meta_data_; }
	inline static void PushMetaTable(lua_State* L) { meta_data_.PushMetaTable(L); }
	inline static int PushObject(lua_State* L, T* obj)
	{
		lua_getglobal(L, "_objects");
		lua_pushlightuserdata(L, (void*)obj);
		lua_rawget(L, -2);
		// if already exist, just get it
		if (lua_isnil(L, -1))
		{
			lua_pop(L, 1);
			LifePolicy::OnAllocated(obj);

			// set metatable to userdata
			UserData<T>** ud = static_cast<UserData<T>**>(lua_newuserdata(L, sizeof(UserData<T>*)));
			*ud = new UserData<T>(obj);
			ScriptMetaClass<T>::PushMetaTable(L);
			lua_setmetatable(L, -2);

			// add userdata to _object table
			lua_pushlightuserdata(L, (void*)obj);
			lua_pushvalue(L, -2);
			lua_rawset(L, -4);
		}
		lua_remove(L, -2);

		return 1;
	}

	static ClassMetaData	meta_data_;
};
template<typename T, typename P > ClassMetaData ScriptMetaClass<T, P>::meta_data_;



// write
//////////////////////////////////////////////////////////////////////////
template<typename T>
struct CppClass2Lua {};

template<typename T>
struct CppClass2Lua<T*>
{
	inline static void Write(lua_State* L, T* ret)
	{
		ScriptMetaClass<T>::PushObject(L, ret);
	}
};

// read
//////////////////////////////////////////////////////////////////////////
template<typename T>
struct Lua2CppClass {};

template<typename T>
struct Lua2CppClass<T*>
{
	inline static T* Read(lua_State* L, int index)
	{
		UserData<T>** ud = static_cast<UserData<T>**>(lua_touserdata(L, index));
		return (*ud)->cpp_obj_;
	}
};

//////////////////////////////////////////////////////////////////////////


#endif // !__TEMPLATE_H__
