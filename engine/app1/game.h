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
	void update();

public:
	CScene*	m_pScene;
	bool m_bExit;
	bool m_bBackMode;
};

void createGame();
void destroyGame();

extern CGame* g_pGame;

