#pragma once

class VKeyboard;
extern VKeyboard* gVKeyboard;

bool HookUser32();
bool HookDInput();
bool HookDInput2();
