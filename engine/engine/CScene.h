#pragma once

#include "base.h"
#include "vector3d.h"
#include "rect.h"
#include <string>

class CCamera;

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

	void render3D() const;
	void render2D() const;
	
private:
	std::string m_strName;
	std::unique_ptr<CCamera>	m_p3DCamera;
	std::unique_ptr<CCamera>	m_p2DCamera;
};

