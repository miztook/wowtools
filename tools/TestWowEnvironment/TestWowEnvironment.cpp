#include <crtdbg.h>
#include <stdio.h>
#include <regex>
#include <iostream>

int main(int argc, char* argv[])
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif


	getchar();
	return 0;
}
