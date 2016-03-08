#pragma once

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#ifdef _DEBUG
#pragma comment(lib, "lua5.1d.lib")
#else
#pragma comment(lib, "lua5.1.lib")
#endif

class LuaEng {
public:
	LuaEng();

	bool init();
	void uninit();

	bool loadScript(const char* fileName);

	void createFunction(const char* name, lua_CFunction funcPtr);
	void createConstant(const char* name, int val);
	void createConstant(const char* name, std::string val);
	void createConstant(const char* name, double val);

	bool invoke(const char* funcName, const char* fmt, ...);

	lua_State* getLuaState()
	{
		return _luaState;
	}

	void reset()
	{
		lua_settop(_luaState, 0);
	}

protected:
	lua_State*			_luaState;
};
