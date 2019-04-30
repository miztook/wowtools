#include "base.h"
#include "Engine.h"
#include <crtdbg.h>

#pragma comment(lib, "mywow.lib")

int main()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	


	return 0;
}