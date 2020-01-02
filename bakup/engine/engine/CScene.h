#pragma once

#include "base.h"
#include "vector3d.h"
#include "rect.h"
#include <string>
#include <list>

class CCamera;
class ISceneNode;

class CMeshSceneNode;

class CScene
{
public:
	explicit CScene(const char* strName);
	~CScene();

public:
	const char* getName() const { return m_strName.c_str(); }
	void init3DCamera(float fov, float aspectRatio, float nearZ, float farZ, 
		const vector3df& pos, const vector3df& dir, const vector3df& up);
	void init2DCamera(const dimension2d& screenSize, float vFront, float vBack);

	CCamera* get3DCamera() const { return m_p3DCamera.get(); }
	CCamera* get2DCamera() const { return m_p2DCamera.get(); }
	void onScreenResize(const dimension2d& size);
	
	void cleanupSceneNodes();
	void deleteAllSceneNodes();
	const std::list<ISceneNode*>& getTopSceneNodeList() const { return m_SceneNodes; }

	void afterFrame() { cleanupSceneNodes(); }

	//
	CMeshSceneNode* addMeshSceneNode(const char* name);

private:
	std::string m_strName;
	std::unique_ptr<CCamera>	m_p3DCamera;
	std::unique_ptr<CCamera>	m_p2DCamera;

	std::list<ISceneNode*>	m_SceneNodes;		//所有结点
};

