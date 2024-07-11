#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <cmath>


constexpr float PI = 3.14159265358979323846f;

EulerAngles::EulerAngles()
	: m_yaw(0.f)
	, m_pitch(0.f)
	, m_roll(0.f) 
{

}

EulerAngles::EulerAngles(float yaw, float pitch, float roll)
	: m_yaw(yaw)
	, m_pitch(pitch)
	, m_roll(roll) 
{

}

void EulerAngles::GetAsVectors_XFwd_YLeft_ZUp(Vec3& iForwardBasis, Vec3& jLeftBasis, Vec3& kUpBasis) const 
{
	float yawRad = m_yaw * PI / 180.0f;
	float pitchRad = m_pitch * PI / 180.0f;
	float rollRad = m_roll * PI / 180.0f;

	iForwardBasis.x = cos(pitchRad) * cos(yawRad);
	iForwardBasis.y = cos(pitchRad) * sin(yawRad);
	iForwardBasis.z = -sin(pitchRad);

	jLeftBasis.x = -cos(rollRad) * sin(yawRad) + sin(rollRad) * sin(pitchRad) * cos(yawRad);
	jLeftBasis.y = cos(rollRad) * cos(yawRad) + sin(rollRad) * sin(pitchRad) * sin(yawRad);
	jLeftBasis.z = sin(rollRad) * cos(pitchRad);

	kUpBasis.x = sin(rollRad) * sin(yawRad) + cos(rollRad) * sin(pitchRad) * cos(yawRad);
	kUpBasis.y = -sin(rollRad) * cos(yawRad) + cos(rollRad) * sin(pitchRad) * sin(yawRad);
	kUpBasis.z = cos(rollRad) * cos(pitchRad);
}

Mat44 EulerAngles::GetAsMatrix_XFwd_YLeft_ZUp() const
{
	Mat44 rollMatrix = Mat44::CreateXRotationDegrees(m_roll);
	Mat44 yawMatrix = Mat44::CreateZRotationDegrees(m_yaw);
	Mat44 pitchMatrix = Mat44::CreateYRotationDegrees(m_pitch);
	 
	Mat44 result;

	result.Append(yawMatrix);
	result.Append(pitchMatrix);
	result.Append(rollMatrix);

	return result;
}

Vec3 EulerAngles::GetForwardVector() const {
	float yawRadians = m_yaw * (PI / 180.0f);
	float pitchRadians = -m_pitch * (PI / 180.0f);

	Vec3 forward;
	forward.x = cos(pitchRadians) * cos(yawRadians);
	forward.y = cos(pitchRadians) * sin(yawRadians);
	forward.z = sin(pitchRadians);

	return forward;
}

Vec3 EulerAngles::GetLeftVector() const {
	float yawRadians = m_yaw * (PI / 180.0f);
	return Vec3(-sin(yawRadians), cos(yawRadians), 0.0f);
}

Vec3 EulerAngles::GetWorldUpVector() const
{
	return Vec3(0.0f, 0.0f, 1.0f);;
}

Vec3 EulerAngles::GetLocalUpVector() const
{
	Mat44 rotationMatrix = GetAsMatrix_XFwd_YLeft_ZUp();
	Vec3 initialUpVector(0, 0, 1);
	Vec3 rotatedUpVector = rotationMatrix.TransformPosition3D(initialUpVector);
	return rotatedUpVector;
}
