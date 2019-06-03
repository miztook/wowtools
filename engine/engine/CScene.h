#pragma once

#include "base.h"
#include "vector3d.h"
#include "rect.h"

class CCamera;

class CScene
{
public:
	CScene();
	~CScene();

public:
	void init3DCamera(float fov, float aspectRatio, float nearZ, float farZ, 
		const vector3df& pos, const vector3df& dir, const vector3df& up);
	void init2DCamera(const recti& screenRect, float vFront, float vBack,
		const vector3df& pos, const vector3df& dir, const vector3df& up);

	CCamera* get3DCamera() const { return m_p3DCamera.get(); }
	CCamera* get2DCamera() const { return m_p2DCamera.get(); }

	void render3D();
	void render2D();

private:
	std::unique_ptr<CCamera>	m_p3DCamera;
	std::unique_ptr<CCamera>	m_p2DCamera;
};

