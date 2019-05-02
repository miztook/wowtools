#include "game.h"
#include "Engine.h"

class CInputListener : public IInputListener
{
public:
	CInputListener()
	{
		LastX = LastY = 0;
		LCapture = RCapture = false;
	}
public:
	virtual void onMouseMessage(window_type hwnd, E_INPUT_MESSAGE message, int _x, int _y);
	virtual void onMouseWheel(window_type hwnd, int zDelta);
	virtual void onKeyMessage(window_type hwnd, E_INPUT_MESSAGE message, int key);

private:
	float		LastX;
	float		LastY;
	bool		LCapture;
	bool		RCapture;
};

void CInputListener::onKeyMessage(window_type hwnd, E_INPUT_MESSAGE message, int key)
{
	if (message == Key_Up && key == VK_ESCAPE)
	{
		g_bExit = true;
	}
}

void CInputListener::onMouseMessage(window_type hwnd, E_INPUT_MESSAGE message, int _x, int _y)
{
	
}

void CInputListener::onMouseWheel(window_type hwnd, int zDelta)
{
	if (::GetActiveWindow() != hwnd) return;

}

CInputListener g_listener;

void createInput()
{
	CInputReader* inputReader = g_Engine->getInputReader();
	inputReader->addListener(&g_listener);

	inputReader->acquire(EID_KEYBOARD);
	inputReader->acquire(EID_MOUSE);
}

void destroyInput()
{
	CInputReader* inputReader = g_Engine->getInputReader();
	inputReader->removeListener(&g_listener);
}

void processInput()
{

}