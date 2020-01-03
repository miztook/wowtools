#include "game.h"
#include "Engine.h"
#include "CCamera.h"

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
	const CScene* scene = g_pGame->m_pScene;
	if (!scene)
		return;

	CCamera* cam = scene->get3DCamera();
	if (!cam->IsInited())
		return;

	switch (message)
	{
	case Mouse_LeftButtonDown:
	{
		if (!RCapture)
		{
			LastX = (float)_x;
			LastY = (float)_y;
			SetCapture(hwnd);
			LCapture = true;
		}
	}
	break;
	case Mouse_LeftButtonUp:
	{
		if (LCapture)
		{
			LastX = LastY = 0;
			ReleaseCapture();
			LCapture = false;
		}
	}
	break;
	case Mouse_RightButtonDown:
	{
		if (!LCapture)
		{
			LastX = (float)_x;
			LastY = (float)_y;
			SetCapture(hwnd);
			RCapture = true;
		}
	}
	break;
	case Mouse_RightButtonUp:
	{
		if (RCapture)
		{
			LastX = LastY = 0;
			ReleaseCapture();
			RCapture = false;
		}
	}
	break;
	case Mouse_Move:
	{
		float x = (float)_x;
		float y = (float)_y;

		const float blend = 0.8f;				//Ðý×ªÈáºÍ

		if (LCapture || RCapture)
		{
			float deltaX = x - LastX;
			float deltaY = y - LastY;

			bool xChange = fabs(deltaX) > 0.5f;
			bool yChange = fabs(deltaY) > 0.5f;

			if (xChange || yChange)
			{
				float blendDeltaX = xChange ? deltaX * blend : deltaX;
				float blendDeltaY = yChange ? deltaY * blend : deltaY;

				LastX += blendDeltaX;
				LastY += blendDeltaY;

				if (LCapture)
					cam->pitch_yaw_Maya(blendDeltaY * 0.3f, blendDeltaX * 0.3f, vector3df(0,0,0));
				else
					cam->move_offset_Maya(blendDeltaX * 0.02f, blendDeltaY * 0.02f);

				//					cam->pitch_yaw_FPS(blendDeltaY * 0.2f, blendDeltaX * 0.2f);
			}
		}
	}
	break;
	default:
		break;
	}
}

void CInputListener::onMouseWheel(window_type hwnd, int zDelta)
{
	if (::GetActiveWindow() != hwnd) return;

	const CScene* scene = g_pGame->m_pScene;
	if (!scene)
		return;

	CCamera* cam = scene->get3DCamera();
	if (!cam->IsInited())
		return;

	CCamera::SKeyControl keyControl = { 0 };
	keyControl.front = zDelta > 0;
	keyControl.back = zDelta < 0;

	cam->onKeyMove(0.01f * abs(zDelta), keyControl);

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

void CGame::processInput()
{
	const CScene* scene = g_pGame->m_pScene;
	if (!scene)
		return;

	CCamera* cam = scene->get3DCamera();
	if (!cam->IsInited())
		return;

	static TIME_POINT startTime = CSysChrono::getTimePointNow();
	TIME_POINT now = CSysChrono::getTimePointNow();
	uint32_t delta = CSysChrono::getDurationMilliseconds(now, startTime);
	uint32_t time = min_(delta, 500u);
	startTime = now;

	CInputReader* inputReader = g_Engine->getInputReader();
	CCamera::SKeyControl keyControl;
	keyControl.front = inputReader->isKeyPressed('W');
	keyControl.back = inputReader->isKeyPressed('S');
	keyControl.left = inputReader->isKeyPressed('A');
	keyControl.right = inputReader->isKeyPressed('D');
	keyControl.up = inputReader->isKeyPressed(VK_SPACE);
	keyControl.down = false;

	cam->onKeyMove(0.02f * time, keyControl);
}

