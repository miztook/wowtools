#pragma once
#include "base.h"

class CInputReader;

//globals
extern CInputReader*	g_InputReader;

//
bool createEngine(const SWindowInfo& wndInfo);

void destroyEngine();