#pragma once
#include "mywow.h"

class MyMessageHandler : public IMessageHandler
{
public:
	virtual void onExit(window_type hwnd) override;
	virtual void onSize(window_type hwnd, int width, int height) override;
};

class CGame
{
public:
	CGame();
	~CGame();

public:
	void createScene();

	void createInput();
	void destroyInput();
	void processInput();

	void update();

	void onKeyMessage(window_type hwnd, E_INPUT_MESSAGE message, int key);

public:
	CScene*	m_pScene;
	bool m_bExit;
	bool m_bBackMode;
};

void createGame();
void destroyGame();

extern CGame* g_pGame;

