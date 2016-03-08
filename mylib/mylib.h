#pragma once

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0501	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif						

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <windows.h>
#include <tchar.h>

#include <assert.h>

#include <vector>
#include <map>
#include <list>
#include <set>
#include <queue>
#include <string>

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#endif

#include <guiddef.h>

#include "detours.h"
#include "detoursex.h"
#include "Utils/Debug.h"

#ifdef MYLIB_EXPORT
#define MYAPI __declspec(dllexport)
#else
#define MYAPI __declspec(dllimport)
#endif

