#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/AABB2.hpp"

class Camera
{
public:
	enum Mode
	{
		eMode_Orthogarphic,
		eMode_Perspective,
		eMode_Count
	};

	Camera() {};
	~Camera() {};

	void SetOrthoView(Vec2 const& bottomLeft, Vec2 const& topRight);

	void SetOrthographicView(Vec2 const& bottomLeft, Vec2 const& topRight, float near = 0.0f, float far = 1.0f);
	void SetPerspectiveView(float aspect, float fov, float near, float far);

	Vec2 GetOrthographicBottomLeft() const;
	Vec2 GetOrthographicTopRight() const;
	void Translate2D(Vec2 const& translation);
	void SetRenderBasis(Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& kBasis);
	void SetTransform(const Vec3& position, const EulerAngles& orientation);
	Mat44 GetRenderMatrix() const;
	Mat44 GetViewMatrix() const;
	Mat44 GetModelMatrix() const;

	Mat44 GetOrthographicMatrix() const;
	Mat44 GetPerspectiveMatrix() const;
	Mat44 GetProjectionMatrix() const;

	void SetViewport(Vec2 const& min, Vec2 const& max);

	

public:
	AABB2 m_viewport;
	Vec3 m_position;
	EulerAngles m_orientation;

private:
	Mode m_mode = eMode_Orthogarphic;

	Vec2 m_orthographicBottomLeft;
	Vec2 m_orghographicTopRight;
	float m_orthographicNear;
	float m_orthographicFar;

	float m_perspectiveAspect;
	float m_perspectiveFOV;
	float m_perspectiveNear;
	float m_perspectiveFar;

	Vec3 m_renderIBasis = Vec3(1.0f, 0.0f, 0.0f);
	Vec3 m_renderJBasis = Vec3(0.0f, 1.0f, 0.0f);
	Vec3 m_renderKBasis = Vec3(0.0f, 0.0f, 1.0f);
};



