#include "Engine/Renderer/Camera.hpp"

void Camera::SetOrthoView(Vec2 const& bottomLeft, Vec2 const& topRight)
{
	m_orthographicBottomLeft = bottomLeft;
	m_orghographicTopRight = topRight;
}

void Camera::SetOrthographicView(Vec2 const& bottomLeft, Vec2 const& topRight, float near, float far) {
	m_mode = eMode_Orthogarphic;
	m_orthographicBottomLeft = bottomLeft;
	m_orghographicTopRight = topRight;
	m_orthographicNear = near;
	m_orthographicFar = far;
}

void Camera::SetPerspectiveView(float aspect, float fov, float near, float far) {
	m_mode = eMode_Perspective;
	m_perspectiveAspect = aspect;
	m_perspectiveFOV = fov;
	m_perspectiveNear = near;
	m_perspectiveFar = far;
}

Vec2 Camera::GetOrthographicBottomLeft() const
{
	return m_orthographicBottomLeft;
}

Vec2 Camera::GetOrthographicTopRight() const
{
	return m_orghographicTopRight;
}

void Camera::Translate2D(Vec2 const& translation)
{
	m_orthographicBottomLeft += translation;
	m_orghographicTopRight += translation;
}

void Camera::SetRenderBasis(Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& kBasis)
{
	m_renderIBasis = iBasis;
	m_renderJBasis = jBasis;
	m_renderKBasis = kBasis;
}

void Camera::SetTransform(const Vec3& position, const EulerAngles& orientation)
{
	m_position = position;
	m_orientation = orientation;
}

Mat44 Camera::GetRenderMatrix() const
{
	Mat44 renderMatrix;
	renderMatrix.SetIJKT3D(m_renderIBasis, m_renderJBasis, m_renderKBasis, Vec3(0.0f, 0.0f, 0.0f));
	return renderMatrix;
}

Mat44 Camera::GetOrthographicMatrix() const {
	return Mat44::CreateOrthoProjection(
		m_orthographicBottomLeft.x, m_orghographicTopRight.x,
		m_orthographicBottomLeft.y, m_orghographicTopRight.y,
		m_orthographicNear, m_orthographicFar
	);
}

Mat44 Camera::GetPerspectiveMatrix() const {
	return Mat44::CreatePerspectiveProjection(
		m_perspectiveFOV, m_perspectiveAspect,
		m_perspectiveNear, m_perspectiveFar
	);
}

Mat44 Camera::GetProjectionMatrix() const 
{
	Mat44 projectionMatrix;
	if (m_mode == eMode_Orthogarphic)
	{
		projectionMatrix = GetOrthographicMatrix();
	}
	else if (m_mode == eMode_Perspective)
	{
		projectionMatrix = GetPerspectiveMatrix();
	}
	Mat44 renderMatrix = GetRenderMatrix();
	projectionMatrix.Append(renderMatrix);
	return projectionMatrix;
}

void Camera::SetViewport(Vec2 const& min, Vec2 const& max)
{
	m_viewport.m_mins = min;
	m_viewport.m_maxs = max;
}

Mat44 Camera::GetViewMatrix() const
{
	Mat44 viewMatrix;
	viewMatrix = m_orientation.GetAsMatrix_XFwd_YLeft_ZUp();
	viewMatrix.SetTranslation3D(m_position);
	return viewMatrix.GetOrthonormalInverse();
}

Mat44 Camera::GetModelMatrix() const
{
	Mat44 modelMatrix;
	modelMatrix.Append(m_orientation.GetAsMatrix_XFwd_YLeft_ZUp());
	modelMatrix.Append(Mat44::CreateTranslation3D(m_position));
	return modelMatrix;
}
