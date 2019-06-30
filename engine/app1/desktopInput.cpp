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
	void onMouseMessage(window_type hwnd, E_INPUT_MESSAGE message, int _x, int _y) override;
	void onMouseWheel(window_type hwnd, int zDelta) override;
	void onKeyMessage(window_type hwnd, E_INPUT_MESSAGE message, int key) override;

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
		g_pGame->m_bExit = true;
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

void CGame::createInput()
{
	CInputReader* inputReader = g_Engine->getInputReader();
	inputReader->addListener(&g_listener);

	inputReader->acquire(EID_KEYBOARD);
	inputReader->acquire(EID_MOUSE);
}

void CGame::destroyInput()
{
	CInputReader* inputReader = g_Engine->getInputReader();
	inputReader->removeListener(&g_listener);
}

