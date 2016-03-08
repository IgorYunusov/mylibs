#include "mylib.h"
#include "LuaEng.h"

//////////////////////////////////////////////////////////////////////////

LuaEng::LuaEng()
{
	_luaState = NULL;
}

bool LuaEng::init()
{
	_luaState = lua_open();

	if (_luaState == NULL) {
		return false;
	}

	luaL_openlibs(_luaState);
	return true;
}

void LuaEng::uninit()
{
	lua_close(_luaState);
}

bool LuaEng::loadScript(const char* fileName)
{
	if (luaL_loadfile(_luaState, fileName) != 0) {		

		// log error
		return false;
	}

	if (lua_pcall(_luaState, 0, 0, 0) != 0) {

		// log error
		return false;
	}

	//if (!invoke("LuaInit", "")) {

	//	// log error
	//	return false;
	//}

	return true;
}

void LuaEng::createFunction(const char* name, lua_CFunction funcPtr)
{
	lua_register(_luaState, name, funcPtr);
}

void LuaEng::createConstant(const char* name, int val)
{
	lua_pushinteger(_luaState, val);
	lua_setglobal(_luaState, name);
}

void LuaEng::createConstant(const char* name, std::string val)
{
	lua_pushstring(_luaState, val.c_str());
	lua_setglobal(_luaState, name);
}

void LuaEng::createConstant(const char* name, double val)
{
	lua_pushnumber(_luaState, val);
	lua_setglobal(_luaState, name);
}

/*++

Routine Description:
call a function of lua

Arguments:
	funcName - function name
	fmt - describe the parameters and results.
	%b - boolean
	%d - integer
	%s - string
	%p - user data
	%h - handle (64 bit)
	%n - number
	| - begin of result
-- */

#pragma warning(push)
#pragma warning(disable: 4996)

bool LuaEng::invoke(const char* funcName, const char* fmt, ...)
{
	if (_luaState == NULL)
		return false;

	lua_getglobal(_luaState, funcName);

	int pc = 0;
	int paramCount = 0;
	int resultCount = 0;
	bool foundPipeline = false;
	const char* c = fmt;
	const char* pp;

	va_list vlist;
	va_start(vlist, fmt);

	bool nearPercent = false;

	while (*c) {
		if (nearPercent) {

			switch (*c) {
			case 'b': // boolean

				if (foundPipeline) {
					resultCount ++;

				} else {
					bool p = va_arg(vlist, bool);
					lua_pushboolean(_luaState, p);
					paramCount ++;
				}

				break;

			case 'd': // interger

				if (foundPipeline) {
					resultCount ++;

				} else {
					long p = va_arg(vlist, long);
					lua_pushinteger(_luaState, p);
					paramCount ++;
				}

				break;

			case 's': // string

				if (foundPipeline) {
					resultCount ++;

				} else {
					char* p = va_arg(vlist, char*);
					lua_pushstring(_luaState, p);
					paramCount ++;
				}

				break;

			case 'p': // pointer

				if (foundPipeline) {
					resultCount ++;

				} else {
					void* p = va_arg(vlist, char*);
					lua_pushlightuserdata(_luaState, p);
					paramCount ++;
				}

				break;

			case 'h':

				if (foundPipeline) {
					resultCount ++;

				} else {
					lua_Number n = va_arg(vlist, lua_Number);
					lua_pushnumber(_luaState, n);
					paramCount ++;
				}

				break;

			case 'n':

				if (foundPipeline) {
					resultCount ++;

				} else {
					lua_Number n = va_arg(vlist, lua_Number);
					lua_pushnumber(_luaState, n);
					paramCount ++;
				}

				break;

			default:
				return false; // invalid char
			}
		}

		if (*c == '|') {
			if (foundPipeline)
				return false; // invalid format
			else {

				pp = c + 1;
				foundPipeline = true;
			}
		}

		nearPercent = *c == '%';
		c ++;
	}

	try {
		if (lua_pcall(_luaState, paramCount, resultCount, 0)) {

			// log error
			return false;
		}

	} catch (...) {

		return false;
	}

	if (foundPipeline) {

		c = pp;
		nearPercent = false;

		int rc = - resultCount;

		while (*c) {

			if (nearPercent) {

				switch (*c) {
				case 'b': // boolean

					{
						bool* r = va_arg(vlist, bool* );
						*r = lua_toboolean(_luaState, rc ++) ? true : false;
					}

					break;

				case 'd': // interger

					{
						long* r = va_arg(vlist, long* );
						*r = (long )lua_tointeger(_luaState, rc ++);
					}

					break;

				case 's': // string

					{
						char* r = va_arg(vlist, char* );
						strcpy(r, lua_tostring(_luaState, rc ++));
					}

					break;

				case 'p': // pointer

					{
						void** r = va_arg(vlist, void** );
						*r = lua_touserdata(_luaState, rc ++);
					}

					break;

				case 'h':
					{
						lua_Number* r = va_arg(vlist, lua_Number *);
						*r = lua_tonumber(_luaState, rc ++);
					}

					break;

				case 'n':
					{
						lua_Number* r = va_arg(vlist, lua_Number *);
						*r = lua_tonumber(_luaState, rc ++);
					}

					break;

				default:
					return false; // invalid char
				}
			}

			nearPercent = *c == '%';
			c ++;
		}
	}

	va_end(vlist);
	return true;
}

#pragma warning(pop)

// #define _UNIT_TEST

#ifdef _UNIT_TEST
static void test()
{
	long a;
	LuaEng::instanse().invoke("SayHello", "%s|%d", "Hello, World", &a)	;
}

#endif
