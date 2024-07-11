#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <cmath>
constexpr float PI = 3.14159265358979323846f;

 Mat44::Mat44()
{
	 m_values[Ix] = 1.f; m_values[Iy] = 0.f; m_values[Iz] = 0.f; m_values[Iw] = 0.f;
	 m_values[Jx] = 0.f; m_values[Jy] = 1.f; m_values[Jz] = 0.f; m_values[Jw] = 0.f;
	 m_values[Kx] = 0.f; m_values[Ky] = 0.f; m_values[Kz] = 1.f; m_values[Kw] = 0.f;
	 m_values[Tx] = 0.f; m_values[Ty] = 0.f; m_values[Tz] = 0.f; m_values[Tw] = 1.f;
}

 Mat44::Mat44(Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translation2D)
 {
	 m_values[Ix] = iBasis2D.x;			m_values[Iy] = iBasis2D.y;		m_values[Iz] = 0.f; m_values[Iw] = 0.f;
	 m_values[Jx] = jBasis2D.x;			m_values[Jy] = jBasis2D.y;		m_values[Jz] = 0.f; m_values[Jw] = 0.f;
	 m_values[Kx] = 0.f;				m_values[Ky] = 0.f;				m_values[Kz] = 1.f; m_values[Kw] = 0.f;
	 m_values[Tx] = translation2D.x;	m_values[Ty] = translation2D.y; m_values[Tz] = 0.f; m_values[Tw] = 1.f;
 }

 Mat44::Mat44(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translation3D)
 {
	 m_values[Ix] = iBasis3D.x;			m_values[Iy] = iBasis3D.y;		m_values[Iz] = iBasis3D.z;		m_values[Iw] = 0.f;
	 m_values[Jx] = jBasis3D.x;			m_values[Jy] = jBasis3D.y;		m_values[Jz] = jBasis3D.z;		m_values[Jw] = 0.f;
	 m_values[Kx] = kBasis3D.x;			m_values[Ky] = kBasis3D.y;		m_values[Kz] = kBasis3D.z;		m_values[Kw] = 0.f;
	 m_values[Tx] = translation3D.x;	m_values[Ty] = translation3D.y; m_values[Tz] = translation3D.z; m_values[Tw] = 1.f;
 }

 Mat44::Mat44(Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D)
 {
	 m_values[Ix] = iBasis4D.x;			m_values[Iy] = iBasis4D.y;		m_values[Iz] = iBasis4D.z;		m_values[Iw] = iBasis4D.w;
	 m_values[Jx] = jBasis4D.x;			m_values[Jy] = jBasis4D.y;		m_values[Jz] = jBasis4D.z;		m_values[Jw] = jBasis4D.w;
	 m_values[Kx] = kBasis4D.x;			m_values[Ky] = kBasis4D.y;		m_values[Kz] = kBasis4D.z;		m_values[Kw] = kBasis4D.w;
	 m_values[Tx] = translation4D.x;	m_values[Ty] = translation4D.y; m_values[Tz] = translation4D.z; m_values[Tw] = translation4D.w;
 }

 Mat44::Mat44(float const* sixteenValueBasisMajor)
 {
	 for (int i = 0; i < 16; ++i) 
	 {
		 m_values[i] = sixteenValueBasisMajor[i];
	 }
 }

Mat44 const Mat44::CreateTranslation2D(Vec2 const& translationXY)
 {
	 return Mat44(Vec2(1.f, 0.f), Vec2(0.f, 1.f), translationXY);
 }

Mat44 const Mat44::CreateTranslation3D(Vec3 const& translationXYZ)
 {
	return Mat44(Vec3(1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f), Vec3(0.f, 0.f, 1.f), translationXYZ);
 }

Mat44 const Mat44::CreateUniformScale2D(float uniformScaleXY)
 {
	return Mat44(Vec2(uniformScaleXY, 0.f), Vec2(0.f, uniformScaleXY), Vec2(0.f, 0.f));
 }

Mat44 const Mat44::CreateUniformScale3D(float uniformScaleXYZ)
 {
	return Mat44(Vec3(uniformScaleXYZ, 0.f, 0.f), Vec3(0.f, uniformScaleXYZ, 0.f), Vec3(0.f, 0.f, uniformScaleXYZ), Vec3(0.f, 0.f, 0.f));
 }

Mat44 const Mat44::CreateNonUniformScale2D(Vec2 const& nonUniformScaleXY)
 {
	return Mat44(Vec2(nonUniformScaleXY.x, 0.f), Vec2(0.f, nonUniformScaleXY.y), Vec2(0.f, 0.f));
 }

Mat44 const Mat44::CreateNonUniformScale3D(Vec3 const& nonUniformScaleXYZ)
 {
	return Mat44(Vec3(nonUniformScaleXYZ.x, 0.f, 0.f), Vec3(0.f, nonUniformScaleXYZ.y, 0.f), Vec3(0.f, 0.f, nonUniformScaleXYZ.z), Vec3(0.f, 0.f, 0.f));
 }

Mat44 const Mat44::CreateZRotationDegrees(float rotationDegreesAboutZ)
 {
	float radians = rotationDegreesAboutZ * (PI / 180.f);
	float cosTheta = cosf(radians);
	float sinTheta = sinf(radians);

	return Mat44(Vec2(cosTheta, sinTheta), Vec2(-sinTheta, cosTheta), Vec2(0.f, 0.f));
 }

Mat44 const Mat44::CreateYRotationDegrees(float rotationDegreesAboutY)
 {
	float radians = rotationDegreesAboutY * (PI / 180.f);
	float cosTheta = cosf(radians);
	float sinTheta = sinf(radians);

	return Mat44(Vec3(cosTheta, 0.f, -sinTheta), Vec3(0.f, 1.f, 0.f), Vec3(sinTheta, 0.f, cosTheta), Vec3(0.f, 0.f, 0.f));
 }

Mat44 const Mat44::CreateXRotationDegrees(float rotationDegreesAboutX)
 {
	float radians = rotationDegreesAboutX * (PI / 180.f);
	float cosTheta = cosf(radians);
	float sinTheta = sinf(radians);

	return Mat44(Vec3(1.f, 0.f, 0.f), Vec3(0.f, cosTheta, sinTheta), Vec3(0.f, -sinTheta, cosTheta), Vec3(0.f, 0.f, 0.f));

 }

Mat44 const Mat44::CreateOrthoProjection(float left, float right, float bottom, float top, float zNear, float zFar)
{
	Mat44 result;
	float Sx = 2 / (right - left);
	float Sy = 2 / (top - bottom);
	float Sz = 1 / (zFar - zNear);

	float Transx = (left + right) / (left - right);
	float Transy = (bottom + top) / (bottom - top);
	float Transz = zNear / (zNear - zFar);

	result.m_values[Ix] = Sx;
	result.m_values[Jy] = Sy;
	result.m_values[Kz] = Sz;

	result.m_values[Tx] = Transx;
	result.m_values[Ty] = Transy;
	result.m_values[Tz] = Transz;

	return result;
}

Mat44 const Mat44::CreatePerspectiveProjection(float fovYDegrees, float aspect, float zNear, float zFar)
{
	Mat44 result;
	result.m_values[Jy] = CosDegrees(fovYDegrees * 0.5f) / SinDegrees(fovYDegrees * 0.5f);  // equals 1 if vertical Field of View is 90
	result.m_values[Ix] = result.m_values[Jy] / aspect; // equals scaleY if screen is square (aspect=1.0); equals 1 if square screen and FOV 90
	result.m_values[Kz] = zFar / (zFar - zNear);
	result.m_values[Kw] = 1.0f; // this puts Z into the W component (in preparation for the hardware w-divide)
	result.m_values[Tz] = (zNear * zFar) / (zNear - zFar);
	result.m_values[Tw] = 0.0f;  // Otherwise we would be putting Z+1 (instead of Z) into the W component
	return result;
}

Vec2 const Mat44::TransformVectorQuantity2D(Vec2 const& vectorQuantityXY) const
 {
	 return Vec2(
		 m_values[Ix] * vectorQuantityXY.x + m_values[Jx] * vectorQuantityXY.y,
		 m_values[Iy] * vectorQuantityXY.x + m_values[Jy] * vectorQuantityXY.y
	 );
 }

 Vec3 const Mat44::TransformVectorQuantity3D(Vec3 const& vectorQuantityXYZ) const
 {
	 return Vec3(
		 m_values[Ix] * vectorQuantityXYZ.x + m_values[Jx] * vectorQuantityXYZ.y + m_values[Kx] * vectorQuantityXYZ.z,
		 m_values[Iy] * vectorQuantityXYZ.x + m_values[Jy] * vectorQuantityXYZ.y + m_values[Ky] * vectorQuantityXYZ.z,
		 m_values[Iz] * vectorQuantityXYZ.x + m_values[Jz] * vectorQuantityXYZ.y + m_values[Kz] * vectorQuantityXYZ.z
	 );
 }

 Vec2 const Mat44::TransformPosition2D(Vec2 const& positionXY) const
 {
	 return Vec2(
		 m_values[Ix] * positionXY.x + m_values[Jx] * positionXY.y + m_values[Tx],
		 m_values[Iy] * positionXY.x + m_values[Jy] * positionXY.y + m_values[Ty]
	 );
 }

 Vec3 const Mat44::TransformPosition3D(Vec3 const& position3D) const
 {
	 return Vec3(
		 m_values[Ix] * position3D.x + m_values[Jx] * position3D.y + m_values[Kx] * position3D.z + m_values[Tx],
		 m_values[Iy] * position3D.x + m_values[Jy] * position3D.y + m_values[Ky] * position3D.z + m_values[Ty],
		 m_values[Iz] * position3D.x + m_values[Jz] * position3D.y + m_values[Kz] * position3D.z + m_values[Tz]
	 );
 }

 Vec4 const Mat44::TransformHomogeneous3D(Vec4 const& homogeneousPoint3D) const
 {
	 return Vec4(
		 m_values[Ix] * homogeneousPoint3D.x + m_values[Jx] * homogeneousPoint3D.y + m_values[Kx] * homogeneousPoint3D.z + m_values[Tx] * homogeneousPoint3D.w,
		 m_values[Iy] * homogeneousPoint3D.x + m_values[Jy] * homogeneousPoint3D.y + m_values[Ky] * homogeneousPoint3D.z + m_values[Ty] * homogeneousPoint3D.w,
		 m_values[Iz] * homogeneousPoint3D.x + m_values[Jz] * homogeneousPoint3D.y + m_values[Kz] * homogeneousPoint3D.z + m_values[Tz] * homogeneousPoint3D.w,
		 m_values[Iw] * homogeneousPoint3D.x + m_values[Jw] * homogeneousPoint3D.y + m_values[Kw] * homogeneousPoint3D.z + m_values[Tw] * homogeneousPoint3D.w
	 );
 }


 float* Mat44::GetAsFloatArray()
 {
	 return m_values;
 }

 float const* Mat44::GetAsFloatArray() const
 {
	 return m_values;
 }

 Vec2 const Mat44::GetIBasis2D() const
 {
	 return Vec2(m_values[Ix], m_values[Iy]);
 }

 Vec2 const Mat44::GetJBasis2D() const
 {
	 return Vec2(m_values[Jx], m_values[Jy]);
 }

 Vec2 const Mat44::GetTranslation2D() const
 {
	 return Vec2(m_values[Tx], m_values[Ty]);
 }

 Vec3 const Mat44::GetIBasis3D() const
 {
	 return Vec3(m_values[Ix], m_values[Iy], m_values[Iz]);
 }

 Vec3 const Mat44::GetJBasis3D() const
 {
	 return Vec3(m_values[Jx], m_values[Jy], m_values[Jz]);
 }

 Vec3 const Mat44::GetKBasis3D() const
 {
	 return Vec3(m_values[Kx], m_values[Ky], m_values[Kz]);
 }

 Vec3 const Mat44::GetTranslation3D() const
 {
	 return Vec3(m_values[Tx], m_values[Ty], m_values[Tz]);
 }

 Vec4 const Mat44::GetIBasis4D() const
 {
	 return Vec4(m_values[Ix], m_values[Iy], m_values[Iz], m_values[Iw]);
 }

 Vec4 const Mat44::GetJBasis4D() const
 {
	 return Vec4(m_values[Jx], m_values[Jy], m_values[Jz], m_values[Jw]);
 }

 Vec4 const Mat44::GetKBasis4D() const
 {
	 return Vec4(m_values[Kx], m_values[Ky], m_values[Kz], m_values[Kw]);
 }

 Vec4 const Mat44::GetTranslation4D() const
 {
	 return Vec4(m_values[Tx], m_values[Ty], m_values[Tz], m_values[Tw]);
 }

 Mat44 const Mat44::GetOrthonormalInverse() const
 {
	 Mat44 Tmatrix;
	 Tmatrix.m_values[Tx] = -1 * m_values[Tx];
	 Tmatrix.m_values[Ty] = -1 * m_values[Ty];
	 Tmatrix.m_values[Tz] = -1 * m_values[Tz];

	 Mat44 Rmatrix;
	 Rmatrix.m_values[Ix] = m_values[Ix];
	 Rmatrix.m_values[Iy] = m_values[Iy];
	 Rmatrix.m_values[Iz] = m_values[Iz];

	 Rmatrix.m_values[Jx] = m_values[Jx];
	 Rmatrix.m_values[Jy] = m_values[Jy];
	 Rmatrix.m_values[Jz] = m_values[Jz];

	 Rmatrix.m_values[Kx] = m_values[Kx];
	 Rmatrix.m_values[Ky] = m_values[Ky];
	 Rmatrix.m_values[Kz] = m_values[Kz];
	
	 Rmatrix.Transpose();

	 Rmatrix.Append(Tmatrix);

	 return Rmatrix;
 }

 void Mat44::SetTranslation2D(Vec2 const& translationXY)
 {
	 m_values[Tx] = translationXY.x;
	 m_values[Ty] = translationXY.y;
	 m_values[Tz] = 0.f;      // Set Z translation to 0
	 m_values[Tw] = 1.f;      // Set W component to 1
 }

 void Mat44::SetTranslation3D(Vec3 const& translationXYZ)
 {
	 m_values[Tx] = translationXYZ.x;
	 m_values[Ty] = translationXYZ.y;
	 m_values[Tz] = translationXYZ.z;
	 m_values[Tw] = 1.f;      // Set W component to 1
 }

 void Mat44::SetIJ2D(Vec2 const& iBasis2D, Vec2 const& jBasis2D)
 {
	 m_values[Ix] = iBasis2D.x; m_values[Iy] = iBasis2D.y; m_values[Iz] = 0.f; m_values[Iw] = 0.f;
	 m_values[Jx] = jBasis2D.x; m_values[Jy] = jBasis2D.y; m_values[Jz] = 0.f; m_values[Jw] = 0.f;
 }

 void Mat44::SetIJT2D(Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translationXY)
 {
	 SetIJ2D(iBasis2D, jBasis2D);
	 SetTranslation2D(translationXY);
 }

 void Mat44::SetIJK3D(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D)
 {
	 m_values[Ix] = iBasis3D.x; m_values[Iy] = iBasis3D.y; m_values[Iz] = iBasis3D.z; m_values[Iw] = 0.f;
	 m_values[Jx] = jBasis3D.x; m_values[Jy] = jBasis3D.y; m_values[Jz] = jBasis3D.z; m_values[Jw] = 0.f;
	 m_values[Kx] = kBasis3D.x; m_values[Ky] = kBasis3D.y; m_values[Kz] = kBasis3D.z; m_values[Kw] = 0.f;
 }

 void Mat44::SetIJKT3D(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translationXYZ)
 {
	 SetIJK3D(iBasis3D, jBasis3D, kBasis3D);
	 SetTranslation3D(translationXYZ);
 }

 void Mat44::SetIJKT4D(Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D)
 {
	 m_values[Ix] = iBasis4D.x;			m_values[Iy] = iBasis4D.y;		m_values[Iz] = iBasis4D.z;		m_values[Iw] = iBasis4D.w;
	 m_values[Jx] = jBasis4D.x;			m_values[Jy] = jBasis4D.y;		m_values[Jz] = jBasis4D.z;		m_values[Jw] = jBasis4D.w;
	 m_values[Kx] = kBasis4D.x;			m_values[Ky] = kBasis4D.y;		m_values[Kz] = kBasis4D.z;		m_values[Kw] = kBasis4D.w;
	 m_values[Tx] = translation4D.x;	m_values[Ty] = translation4D.y; m_values[Tz] = translation4D.z; m_values[Tw] = translation4D.w;
 }

 void Mat44::Transpose()
 {
	 float num = m_values[Jx];
	 m_values[Jx] = m_values[Iy];
	 m_values[Iy] = num;

	 num = m_values[Kx];
	 m_values[Kx] = m_values[Iz];
	 m_values[Iz] = num;

	 num = m_values[Tx];
	 m_values[Tx] = m_values[Iw];
	 m_values[Iw] = num;

	 num = m_values[Ky];
	 m_values[Ky] = m_values[Jz];
	 m_values[Jz] = num;

	 num = m_values[Ty];
	 m_values[Ty] = m_values[Jw];
	 m_values[Jw] = num;

	 num = m_values[Tz];
	 m_values[Tz] = m_values[Kw];
	 m_values[Kw] = num;
 }

 void Mat44::Orthonormalize_IFwd_JLeft_KUp()
 {
	 Vec3 iBasis(m_values[Ix], m_values[Iy], m_values[Iz]);
	 iBasis = iBasis.GetNormalized();

	 // Make kBasis orthogonal to iBasis
	 Vec3 kBasis(m_values[Kx], m_values[Ky], m_values[Kz]);
	 Vec3 projKOnI = iBasis * DotProduct3D(kBasis, iBasis);
	 kBasis = kBasis - projKOnI;
	 kBasis = kBasis.GetNormalized(); // Normalize kBasis after making it orthogonal to iBasis

	// Adjust jBasis to be orthogonal to both iBasis and kBasis
	 Vec3 jBasis(m_values[Jx], m_values[Jy], m_values[Jz]);
	 Vec3 projJOnI = iBasis * DotProduct3D(jBasis, iBasis);
	 Vec3 projJOnK = kBasis * DotProduct3D(jBasis, kBasis);
	 jBasis = jBasis - projJOnI - projJOnK;
	 jBasis = jBasis.GetNormalized(); // Normalize jBasis

	// Store the orthonormalized vectors back into the matrix
	 m_values[Ix] = iBasis.x; m_values[Iy] = iBasis.y; m_values[Iz] = iBasis.z;
	 m_values[Jx] = jBasis.x; m_values[Jy] = jBasis.y; m_values[Jz] = jBasis.z;
	 m_values[Kx] = kBasis.x; m_values[Ky] = kBasis.y; m_values[Kz] = kBasis.z;
 }

 void Mat44::Append(Mat44 const& appendThis)
 {
	 Mat44 copyOfThis = *this; // make a copy of my old values, so we dont pollute them as we go
	 float const* left = &copyOfThis.m_values[0]; // column-notation nickname for the prior matrix, for brevity
	 float const* right = &appendThis.m_values[0]; // column-notation nickname for the new matrix, for brevity

	 m_values[Ix] = ((left[Ix] * right[Ix]) + (left[Jx] * right[Iy]) + (left[Kx] * right[Iz]) + (left[Tx] * right[Iw])); // Dot(Lx, Ri)
	 m_values[Iy] = ((left[Iy] * right[Ix]) + (left[Jy] * right[Iy]) + (left[Ky] * right[Iz]) + (left[Ty] * right[Iw]));
	 m_values[Iz] = ((left[Iz] * right[Ix]) + (left[Jz] * right[Iy]) + (left[Kz] * right[Iz]) + (left[Tz] * right[Iw]));
	 m_values[Iw] = ((left[Iw] * right[Ix]) + (left[Jw] * right[Iy]) + (left[Kw] * right[Iz]) + (left[Tw] * right[Iw]));

	 m_values[Jx] = ((left[Ix] * right[Jx]) + (left[Jx] * right[Jy]) + (left[Kx] * right[Jz]) + (left[Tx] * right[Jw]));
	 m_values[Jy] = ((left[Iy] * right[Jx]) + (left[Jy] * right[Jy]) + (left[Ky] * right[Jz]) + (left[Ty] * right[Jw]));
	 m_values[Jz] = ((left[Iz] * right[Jx]) + (left[Jz] * right[Jy]) + (left[Kz] * right[Jz]) + (left[Tz] * right[Jw]));
	 m_values[Jw] = ((left[Iw] * right[Jx]) + (left[Jw] * right[Jy]) + (left[Kw] * right[Jz]) + (left[Tw] * right[Jw]));

	 m_values[Kx] = ((left[Ix] * right[Kx]) + (left[Jx] * right[Ky]) + (left[Kx] * right[Kz]) + (left[Tx] * right[Kw]));
	 m_values[Ky] = ((left[Iy] * right[Kx]) + (left[Jy] * right[Ky]) + (left[Ky] * right[Kz]) + (left[Ty] * right[Kw]));
	 m_values[Kz] = ((left[Iz] * right[Kx]) + (left[Jz] * right[Ky]) + (left[Kz] * right[Kz]) + (left[Tz] * right[Kw]));
	 m_values[Kw] = ((left[Iw] * right[Kx]) + (left[Jw] * right[Ky]) + (left[Kw] * right[Kz]) + (left[Tw] * right[Kw]));

	 m_values[Tx] = ((left[Ix] * right[Tx]) + (left[Jx] * right[Ty]) + (left[Kx] * right[Tz]) + (left[Tx] * right[Tw]));
	 m_values[Ty] = ((left[Iy] * right[Tx]) + (left[Jy] * right[Ty]) + (left[Ky] * right[Tz]) + (left[Ty] * right[Tw]));
	 m_values[Tz] = ((left[Iz] * right[Tx]) + (left[Jz] * right[Ty]) + (left[Kz] * right[Tz]) + (left[Tz] * right[Tw]));
	 m_values[Tw] = ((left[Iw] * right[Tx]) + (left[Jw] * right[Ty]) + (left[Kw] * right[Tz]) + (left[Tw] * right[Tw]));
 }

 void Mat44::AppendZRotation(float degreesRotationAboutZ)
 {
	 Mat44 rotationMatrix = Mat44::CreateZRotationDegrees(degreesRotationAboutZ);
	 Append(rotationMatrix);
 }

 void Mat44::AppendYRotation(float degreesRotationAboutY)
 {
	 Mat44 rotationMatrix = Mat44::CreateYRotationDegrees(degreesRotationAboutY);
	 Append(rotationMatrix);

 }

 void Mat44::AppendXRotation(float degreesRotationAboutX)
 {
	 Mat44 rotationMatrix = Mat44::CreateXRotationDegrees(degreesRotationAboutX);
	 Append(rotationMatrix);
 }

 void Mat44::AppendTranslation2D(Vec2 const& translationXY)
 {
	 Mat44 translationMatrix = Mat44::CreateTranslation2D(translationXY);
	 Append(translationMatrix);
 }

 void Mat44::AppendTranslation3D(Vec3 const& translationXYZ)
 {
	 Mat44 translationMatrix = Mat44::CreateTranslation3D(translationXYZ);
	 Append(translationMatrix);
 }

 void Mat44::AppendScaleUniform2D(float uniformScaleXY)
 {
	 Mat44 scaleMatrix = Mat44::CreateUniformScale2D(uniformScaleXY);
	 Append(scaleMatrix);
 }

 void Mat44::AppendScaleUniform3D(float uniformScaleXYZ)
 {
	 Mat44 scaleMatrix = Mat44::CreateUniformScale3D(uniformScaleXYZ);
	 Append(scaleMatrix);
 }

 void Mat44::AppendScaleNonUniform2D(Vec2 const& nonUniformScaleXY)
 {
	 Mat44 scaleMatrix = Mat44::CreateNonUniformScale2D(nonUniformScaleXY);
	 Append(scaleMatrix);
 }

 void Mat44::AppendScaleNonUniform3D(Vec3 const& nonUniformScaleXYZ)
 {
	 Mat44 scaleMatrix = Mat44::CreateNonUniformScale3D(nonUniformScaleXYZ);
	 Append(scaleMatrix);
 }

