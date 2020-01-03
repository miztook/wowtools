#include "CCamera.h"

#include "function3d.h"

CCamera::CCamera(bool isOrthogonal)
	: m_bInit(false), m_IsOrthogonal(isOrthogonal)
{
	m_vFOV = 0.0f;
	m_vRatio = 0.0f;
	m_vFront = 0.0f;
	m_vBack = 0.0f;
	m_orthoLeft = m_orthoRight = m_orthoLeft = m_orthoRight = 0;

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

void CCamera::Init(float left, float right, float top, float bottom, float vFront, float vBack)
{
	setProjectionParam(left, right, top, bottom, vFront, vBack);
	m_bInit = true;
}

void CCamera::setDirAndUp(const vector3df& vecDir, const vector3df& vecUp)
{
	m_vecDir = f3d::normalize(vecDir);
	vector3df vUp = f3d::normalize(vecUp);
	m_vecRight = f3d::normalize(f3d::crossProduct(vUp, m_vecDir));
	m_vecUp = f3d::normalize(f3d::crossProduct(m_vecDir, m_vecRight));

	updateViewTM();
}

void CCamera::move(const vector3df& vecDelta)
{
	m_vecPos = m_vecPos + vecDelta;
	updateViewTM();
}

void CCamera::turnAroundAxis(const vector3df& vecPos, const vector3df& vecAxis, float vDeltaRad)
{
	quaternion rotate(vecAxis, vDeltaRad);

	m_vecPos = f3d::translate(-vecPos.x, -vecPos.y, -vecPos.z).multiplyPoint(m_vecPos);
	m_vecPos = rotate.multiplyPoint(m_vecPos);
	m_vecPos = f3d::translate(vecPos.x, vecPos.y, vecPos.z).multiplyPoint(m_vecPos);

	m_vecDir = rotate.multiplyPoint(m_vecDir);
	m_vecUp = rotate.multiplyPoint(m_vecUp);

	setDirAndUp(m_vecDir, m_vecUp);
}

void CCamera::setViewTM(const matrix4& matView)
{
	m_matViewTM = matView;
	m_matVPTM = m_matViewTM * m_matProjectionTM;

	vector4df v = matView.getCol(2);
	vector3df vecDir(v.x, v.y, v.z);
	v = matView.getCol(1);
	vector3df vecUp(v.x, v.y, v.z);
	v = matView.getRow(3);
	vector3df vecPos(v.x, v.y, v.z);

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

void CCamera::setProjectionParam(float left, float right, float top, float bottom, float vFront, float vBack)
{
	ASSERT(m_IsOrthogonal);

	m_orthoLeft = left;
	m_orthoRight = right;
	m_orthoTop = top;
	m_orthoBottom = bottom;
	setZFrontAndBack(vFront, vBack);
}

void CCamera::onKeyMove(float speed, const SKeyControl& keycontrol)
{
	if (keycontrol.front || keycontrol.back || keycontrol.left ||
		keycontrol.right || keycontrol.up || keycontrol.down)
	{
		vector3df vDir = getDir();
		vector3df vRight = getRight();
		vector3df vUp = getUp();

		vector3df vDelta(0, 0, 0);
		if (keycontrol.front)
			vDelta += vDir * speed;
		else if (keycontrol.back)
			vDelta -= vDir * speed;

		if (keycontrol.left)
			vDelta -= vRight * speed;
		else if (keycontrol.right)
			vDelta += vRight * speed;

		if (keycontrol.up)
			vDelta += vUp * speed;
		else if (keycontrol.down)
			vDelta -= vUp * speed;

		move(vDelta);
	}
}

void CCamera::pitch_yaw_Maya(float pitchDegree, float yawDegree, const vector3df& vTarget)
{
	quaternion quatX(getRight(), pitchDegree * DEGTORAD);
	quaternion quatY(vector3df::UnitY(), yawDegree * DEGTORAD);
	quaternion q = (quatX * quatY);

	vector3df vOppositeDir = -getDir();
	vOppositeDir = q * vOppositeDir;

	float d = vOppositeDir.dotProduct(vector3df::UnitY());
	if (fabs(d) < 0.9999f)
	{
		turnAroundAxis(vTarget, getRight(), pitchDegree * DEGTORAD);
	}
	turnAroundAxis(vTarget, vector3df::UnitY(), yawDegree * DEGTORAD);
}

void CCamera::move_offset_Maya(float xOffset, float yOffset)
{
	move(-getRight() * xOffset + getUp() * yOffset);
}

void CCamera::pitch_yaw_FPS(float pitchDegree, float yawDegree)
{
	quaternion quatX(getRight(), pitchDegree * DEGTORAD);
	quaternion quatY(vector3df::UnitY(), yawDegree * DEGTORAD);
	quaternion q = (quatX * quatY);

	vector3df v = q * getDir();
	float d = v.dotProduct(vector3df::UnitY());
	if (fabs(d) > 0.9999f)
	{
		v = quatY * getDir();
	}

	setDirAndUp(v, vector3df::UnitY());
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
		m_matProjectionTM = f3d::makeOrthoOffCenterMatrixLH(m_orthoLeft, m_orthoRight, m_orthoTop, m_orthoBottom, m_vFront, m_vBack);
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

		//left
		{
			vector3df vNormal = f3d::viewToWorld(vNormals[frustum::VF_LEFT], m_matViewTM);
			m_worldFrustum.setPlane(frustum::VF_LEFT, plane3df(m_vecPos, vNormal));
		}

		//right
		{
			vector3df vNormal = f3d::viewToWorld(vNormals[frustum::VF_RIGHT], m_matViewTM);
			m_worldFrustum.setPlane(frustum::VF_RIGHT, plane3df(m_vecPos, vNormal));
		}

		//top
		{
			vector3df vNormal = f3d::viewToWorld(vNormals[frustum::VF_TOP], m_matViewTM);
			m_worldFrustum.setPlane(frustum::VF_TOP, plane3df(m_vecPos, vNormal));
		}

		//bottom
		{
			vector3df vNormal = f3d::viewToWorld(vNormals[frustum::VF_BOTTOM], m_matViewTM);
			m_worldFrustum.setPlane(frustum::VF_BOTTOM, plane3df(m_vecPos, vNormal));
		}

		//near
		{
			vector3df vPos = m_vecPos + m_vecDir * m_vFront;
			vector3df vNormal = m_vecDir;
			m_worldFrustum.setPlane(frustum::VF_NEAR, plane3df(vPos, m_vecDir));
		}

		//far
		{
			vector3df vPos = m_vecPos + m_vecDir * m_vBack;
			vector3df vNormal = -m_vecDir;
			m_worldFrustum.setPlane(frustum::VF_FAR, plane3df(vPos, vNormal));
		}
	}
	else
	{
		//left
		{
			vector3df vNormal = m_vecRight;
			m_worldFrustum.setPlane(frustum::VF_LEFT, plane3df(m_vecPos + m_vecRight * m_orthoLeft, vNormal));
		}

		//right
		{
			vector3df vNormal = -m_vecRight;
			m_worldFrustum.setPlane(frustum::VF_RIGHT, plane3df(m_vecPos + m_vecRight * m_orthoRight, vNormal));
		}

		//top
		{
			vector3df vNormal = -m_vecUp;
			m_worldFrustum.setPlane(frustum::VF_TOP, plane3df(m_vecPos + m_vecUp * m_orthoTop, vNormal));
		}

		//bottom
		{
			vector3df vNormal = m_vecUp;
			m_worldFrustum.setPlane(frustum::VF_BOTTOM, plane3df(m_vecPos + m_vecRight * m_orthoBottom, vNormal));
		}

		//near
		{
			vector3df vNormal = m_vecDir;
			m_worldFrustum.setPlane(frustum::VF_LEFT, plane3df(m_vecPos + m_vecDir * m_vFront, vNormal));
		}

		//far
		{
			vector3df vNormal = -m_vecDir;
			m_worldFrustum.setPlane(frustum::VF_LEFT, plane3df(m_vecPos + m_vecRight * m_vBack, vNormal));
		}
	}
}
