#pragma once

#ifndef _WIN32_WINNT		// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define _WIN32_WINNT 0x0501	// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif						

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN		// �� Windows ͷ���ų�����ʹ�õ�����
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

