#include "CCamera.h"

#include "function3d.h"

CCamera::CCamera(bool isOrthogonal)
	: m_bInit(false), m_IsOrthogonal(isOrthogonal)
{
	m_vFOV = 0.0f;
	m_vRatio = 0.0f;
	m_vFront = 0.0f;
	m_vBack = 0.0f;
	m_ScreenRect.set(0, 0, 0, 0);

	m_vecPos = vector3df::Zero();
	m_vecDir = vector3df::UnitZ();
	m_vecUp = vector3df::UnitY();
	m_vecRight = vector3df::UnitX();
}

CCamera::~CCamera()
{

}

void CCamera::Init(float vFov, float vRatio, float vFront, float vBack)
{
	setProjectionParam(vFov, vRatio, vFront, vBack);
	m_bInit = true;
}

void CCamera::Init(const recti& screenRect, float vFront, float vBack)
{
	setProjectionParam(screenRect, vFront, vBack);
	m_bInit = true;
}

void CCamera::setViewTM(const matrix4& matView)
{
	m_matViewTM = matView;
	m_matVPTM = m_matViewTM * m_matProjectionTM;

	vector3df vecDir, vecUp, vecPos;
	vecDir = matView.getCol(2);
	vecUp = matView.getCol(1);
	vecPos = matView.getRow(3);

	matrix4 matInv = matView;
	matInv._41 = matInv._42 = matInv._43 = 0.0f;
	matInv.transpose();

	setDirAndUp(vecDir, vecUp);
	setPos(vecPos);
}

void CCamera::setProjectionTM(const matrix4& matProjection)
{
	m_matProjectionTM = matProjection;

	m_matVPTM = m_matViewTM * m_matProjectionTM;

	updateWorldFrustum();
}

void CCamera::setZFrontAndBack(float vFront, float vBack)
{
	if (vFront > 0.0f)
		m_vFront = vFront;
	if (vBack > 0.0f)
		m_vBack = vBack;

	updateProjectionTM();
}

void CCamera::setProjectionParam(float vFOV, float vRatio, float vFront, float vBack)
{
	ASSERT(!m_IsOrthogonal);

	if (vFOV > 0.0f)
		m_vFOV = vFOV;
	if (vRatio > 0.0f)
		m_vRatio = vRatio;
	setZFrontAndBack(vFront, vBack);
}

void CCamera::setProjectionParam(const recti& screenRect, float vFront, float vBack)
{
	ASSERT(m_IsOrthogonal);

	m_ScreenRect = screenRect;
	setZFrontAndBack(vFront, vBack);
}

void CCamera::updateViewTM()
{
	m_matViewTM = f3d::makeViewMatrix(m_vecPos, m_vecDir, m_vecUp, 0.0f);
	m_matVPTM = m_matViewTM * m_matProjectionTM;

	updateWorldFrustum();
}

void CCamera::updateProjectionTM()
{
	if (!m_IsOrthogonal)
		m_matProjectionTM = f3d::makePerspectiveFovMatrixLH(m_vFOV, m_vRatio, m_vFront, m_vBack);
	else
		m_matProjectionTM = f3d::makeOrthoOffCetnerMatrixLH(m_ScreenRect.left(), m_ScreenRect.right(), m_ScreenRect.top(), m_ScreenRect.bottom());

	m_matVPTM = m_matViewTM * m_matProjectionTM;
	updateWorldFrustum();
}

void CCamera::updateWorldFrustum()
{
	if (!m_IsOrthogonal)
	{
		float c, s, fAngle = m_vFOV * 0.5f;
		c = (float)cos(fAngle);
		s = (float)sin(fAngle);

		vector3df vNormals[6];
		vNormals[frustum::VF_TOP].set(0.0f, -c, s);
		vNormals[frustum::VF_BOTTOM].set(0.0f, c, s);

		fAngle = (float)(tan(fAngle) * m_vRatio);
		c = (float)cos(atan(fAngle));
		s = (float)sin(atan(fAngle));

		vNormals[frustum::VF_LEFT].set(c, 0.0f, s);
		vNormals[frustum::VF_RIGHT].set(-c, 0.0f, s);

		vNormals[frustum::VF_NEAR].set(0.0f, 0.0f, 1.0f);
		vNormals[frustum::VF_FAR].set(0.0f, 0.0f, -1.0f);
	}
	else
	{

	}
}
