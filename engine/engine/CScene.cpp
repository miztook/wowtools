#include "CScene.h"
#include "CCamera.h"

CScene::CScene()
{
	
}

CScene::~CScene()
{

}

void CScene::init3DCamera(float fov, float aspectRatio, float nearZ, float farZ, const vector3df& pos, const vector3df& dir, const vector3df& up)
{
	m_p3DCamera->Init(fov, aspectRatio, nearZ, farZ);
	m_p3DCamera->setPos(pos);
	m_p3DCamera->setDirAndUp(dir, up);
}

void CScene::init2DCamera(const recti& screenRect, float vFront, float vBack, const vector3df& pos, const vector3df& dir, const vector3df& up)
{
	m_p2DCamera->Init(screenRect, vFront, vBack);
	m_p2DCamera->setPos(pos);
	m_p2DCamera->setDirAndUp(dir, up);
}

void CScene::render3D()
{

}

void CScene::render2D()
{

}
