#pragma once
#include "Mat44.hpp"

struct EulerAngles
{

public:
	EulerAngles(); 
	EulerAngles(float yaw, float pitch, float roll);
			
	void GetAsVectors_XFwd_YLeft_ZUp(Vec3& iForwardBasis, Vec3& jLeftBasis, Vec3& kUpBasis) const;
	Mat44 GetAsMatrix_XFwd_YLeft_ZUp() const;
	Vec3 GetForwardVector() const;
	Vec3 GetLeftVector() const;
	Vec3 GetWorldUpVector() const;
	Vec3 GetLocalUpVector() const;
public:
	float m_yaw;   // Y-axis 
	float m_pitch; // X-axis 
	float m_roll;  // Z-axis 
};

