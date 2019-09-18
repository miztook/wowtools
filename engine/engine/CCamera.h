#pragma once

#include "vector3d.h"
#include "matrix4.h"
#include "frustum.h"

class CCamera
{
public:
	explicit CCamera(bool isOrthogonal);
	~CCamera();

public:
	void Init(float vFov, float vRatio, float vFront, float vBack);
	void Init(float left, float right, float top, float bottom, float vFront, float vBack);

	void setPos(const vector3df& vecPos) { m_vecPos = vecPos; updateViewTM(); }
	void setDirAndUp(const vector3df& vecDir, const vector3df& vecUp);
	void move(const vector3df& vecDelta);
	void turnAroundAxis(const vector3df& vecPos, const vector3df& vecAxis, float vDeltaRad);

	void setViewTM(const matrix4& matView);
	void setProjectionTM(const matrix4& matProjection);
	void setZFrontAndBack(float vFront, float vBack);

	void setFOV(float vFOV) { m_vFOV = vFOV; updateProjectionTM(); }
	void setProjectionParam(float vFOV, float vRatio, float vFront, float vBack);
	void setProjectionParam(float left, float right, float top, float bottom, float vFront, float vBack);

	const frustum& getWorldFrustum() const { return m_worldFrustum; }

public:
	bool IsOrthogonal() const { return m_IsOrthogonal; }
	bool IsInited() const { return m_bInit; }

	float getZFront() const { return m_vFront; }
	float getZBack() const { return m_vBack; }

	const matrix4& getViewTM() const { return m_matViewTM; }
	const matrix4& getProjectionTM() const { return m_matProjectionTM; }
	const matrix4& getVPTM() const { return m_matVPTM; }
	const vector3df& getPos() const { return m_vecPos; }
	const vector3df& getDir() const { return m_vecDir; }
	const vector3df& getRight() const { return m_vecRight; }

	float getFOV() const { return m_vFOV; }
	float getRatio() const { return m_vRatio; }

	void getProjectionParam(float* pvFOV, float* pvRatio, float* pvFront, float* pvBack)
	{
		*pvFOV = m_vFOV;
		*pvFront = m_vFront;
		*pvBack = m_vBack;
		*pvRatio = m_vRatio;
	}

private:
	void updateViewTM();
	void updateProjectionTM();
	void updateWorldFrustum();

private:
	vector3df m_vecPos;
	vector3df m_vecDir;
	vector3df m_vecUp;
	vector3df m_vecRight;

	matrix4 m_matViewTM;
	matrix4 m_matProjectionTM;
	matrix4 m_matVPTM;

	float m_vFront, m_vBack;
	union 
	{
		struct
		{
			float m_vFOV;
			float m_vRatio;
		};
		struct  
		{
			float m_orthoLeft;
			float m_orthoRight;
			float m_orthoTop;
			float m_orthoBottom;
		};
	};

	frustum	m_worldFrustum;

	const bool m_IsOrthogonal;
	bool m_bInit;
};