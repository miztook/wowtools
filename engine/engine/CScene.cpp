#include "CScene.h"
#include "CCamera.h"
#include "Engine.h"

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

void CScene::onScreenResize(const dimension2d& size)
{
	if (m_p3DCamera->IsInited())
	{
		float aspect = (float)size.width / (float)size.height;
		m_p3DCamera->setProjectionParam(m_p3DCamera->getFOV(), aspect, m_p3DCamera->getZFront(), m_p3DCamera->getZBack());
	}

	if (m_p2DCamera->IsInited())
	{
		recti rect(0, 0, size.width, size.height);
		m_p2DCamera->setProjectionParam(rect, m_p2DCamera->getZFront(), m_p2DCamera->getZBack());
	}
}

void CScene::render3D() const
{
	if (!m_p3DCamera->IsInited())
		return;
}

void CScene::render2D() const
{
	if (!m_p2DCamera->IsInited())
		return;

}

void CScene::renderDebugInfo() const
{
	if (!m_p2DCamera->IsInited())
		return;

}
