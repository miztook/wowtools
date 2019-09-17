#include "CScene.h"
#include "CCamera.h"
#include "Engine.h"
#include "ISceneNode.h"

CScene::CScene(const char* strName)
	: m_strName(strName)
{
	m_p3DCamera = std::make_unique<CCamera>(false);
	m_p2DCamera = std::make_unique<CCamera>(true);
}

CScene::~CScene()
{
	m_p2DCamera.reset();
	m_p3DCamera.reset();
}

void CScene::init3DCamera(float fov, float aspectRatio, float nearZ, float farZ, const vector3df& pos, const vector3df& dir, const vector3df& up)
{
	m_p3DCamera->Init(fov, aspectRatio, nearZ, farZ);
	m_p3DCamera->setPos(pos);
	m_p3DCamera->setDirAndUp(dir, up);
}

void CScene::init2DCamera(const dimension2d& screenSize, float vFront, float vBack)
{
	float fHalfWidth = screenSize.width * 0.5f; 
	float fHalfHeight = screenSize.height * 0.5f;
	m_p2DCamera->Init(-fHalfWidth, fHalfWidth, -fHalfHeight, fHalfHeight, vFront, vBack);
	m_p2DCamera->setPos(vector3df(fHalfWidth, fHalfHeight, vFront));
	m_p2DCamera->setDirAndUp(vector3df::UnitZ(), vector3df::UnitY());
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
		float fHalfWidth = size.width * 0.5f;
		float fHalfHeight = size.height * 0.5f;
		m_p2DCamera->setProjectionParam(-fHalfWidth, fHalfWidth, -fHalfHeight, fHalfHeight, m_p2DCamera->getZFront(), m_p2DCamera->getZBack());
		m_p2DCamera->setPos(vector3df(fHalfWidth, fHalfHeight, m_p2DCamera->getZFront()));
		m_p2DCamera->setDirAndUp(vector3df::UnitZ(), vector3df::UnitY());
	}
}

void CScene::addSceneNode(ISceneNode* node)
{
	if (node && node->getTransform()->getParent())
		m_SceneNodes.push_back(node);
}

void CScene::cleanSceneNodes()
{
	for (auto itr = m_SceneNodes.begin(); itr != m_SceneNodes.end();)
	{
		ISceneNode* node = *itr;
		if (node->isToDelete())
		{
			ISceneNode::checkDelete(node);
			m_SceneNodes.erase(itr++);
		}
		else
		{
			++itr;
		}
	}
}

void CScene::deleteAllSceneNodes()
{
	for (auto itr = m_SceneNodes.begin(); itr != m_SceneNodes.end();)
	{
		ISceneNode* node = *itr;
		delete node;
	}
	m_SceneNodes.clear();
}


