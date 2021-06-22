
#pragma once

#include "EngineBaseGlobals/EngineBaseApiPrefix.h"


    

#define GLM_FORCE_RADIANS
#define GLM_FORECE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_SILENT_WARNINGS

/*
Inception Engine use a right-hand coordinate system. positive-x is to the right,
positive-y is to the up, and positive-z goes out of the screen.
*/
#define GLM_FORCE_RIGHT_HANDED

#include "External/glm/glm.hpp"
#include "External/glm/gtc/matrix_transform.hpp"
#include "External/glm/gtx/string_cast.hpp"
#include "External/glm/gtx/quaternion.hpp"
#include "External/glm/gtx/rotate_vector.hpp"
#include "External/glm/gtx/matrix_decompose.hpp"
#include "External/glm/gtc/matrix_inverse.hpp"

#include "External/glm/gtx/euler_angles.hpp"

#include <vector>
#include <random>

namespace inceptionengine
{

	using Vec2f = glm::vec2;
	using Vec3f = glm::vec3;
	using Vec4f = glm::vec4;
	using Vec4ui = glm::uvec4;

	using Matrix4x4f = glm::mat4x4;
	using Quaternion4f = glm::quat;

	float constexpr PI = glm::pi<float>();

	inline Matrix4x4f LookAt(Vec3f const& pos, Vec3f const& forward, Vec3f const& up)
	{
		return glm::lookAt(pos, forward, up);
	}

	inline Matrix4x4f Perspective(float degrees, float aspect, float near, float far)
	{
		return glm::perspective(glm::radians(degrees), aspect, near, far);
	}

	inline Matrix4x4f Inverse(Matrix4x4f const& mat)
	{
		return glm::inverse(mat);
	}

	inline Matrix4x4f AffineInverse(Matrix4x4f const& mat)
	{
		return glm::affineInverse(mat);
	}

	inline Vec3f NormalizeVec(Vec3f const& vec)
	{
		return glm::normalize(vec);
	}

	inline Vec4f NormalizeVec(Vec4f const& vec)
	{
		return glm::normalize(vec);
	}

	inline float VecLength(Vec4f const& vec)
	{
		return glm::length(vec);
	}

	inline float VecLength(Vec3f const& vec)
	{
		return glm::length(vec);
	}

	inline Vec3f CrossProduct(Vec3f const& u, Vec3f const& v)
	{
		return glm::cross(u, v);
	}

	inline Vec4f Rotate(Vec4f const& point, float degree, Vec3f const& axis)
	{
		return glm::rotate(point, glm::radians(degree), axis);
	}

	inline Vec4f RotateVec(Vec3f const& vec, float degree, Vec3f const& axis)
	{
		Vec4f vector = Vec4f(vec[0], vec[1], vec[2], 0.0f);
		return glm::rotate(vector, glm::radians(degree), axis);
	}

	inline Vec4f RotatePoint(Vec3f const& vec, float degree, Vec3f const& axis)
	{
		Vec4f point = Vec4f(vec[0], vec[1], vec[2], 1.0f);
		return glm::rotate(point, glm::radians(degree), axis);
	}

	inline Matrix4x4f Rotate(float angleInRad, Vec3f const& axis)
	{
		return glm::rotate(angleInRad, axis);
	}

	inline Matrix4x4f Translate(Vec3f const& vec)
	{
		return glm::translate(vec);
	}
	

	inline float Sign(float x)
	{
		if (x >= 0) return 1;
		else return -1;
	}

	inline float Acos(float f)
	{
		return glm::acos(f);
	}

	inline Vec3f RotationAxis(Quaternion4f const& quat)
	{
		return NormalizeVec(Vec3f(quat.x, quat.y, quat.z));
	}

	inline float RotationAngle(Quaternion4f const& quat)
	{
		float rads = 2 * std::acosf(quat.w);
		assert(! std::isnan(rads));
		return rads;
	}

	inline Quaternion4f RotToQuat(Matrix4x4f const& mat)
	{
		return glm::quat_cast(mat);
	}

	inline void NormalizeRotation(float& rads, Vec3f& axis)
	{

		rads = std::fmodf(rads, 2 * PI);
		if (rads < 0.0f)
			rads = rads + 2 * PI;

		if (rads > PI)
		{
			rads = 2 * PI - rads;
			axis = -axis;
		}
			
	}


	inline Matrix4x4f Scale(float x, float y, float z)
	{
		return glm::scale(Vec3f(x, y, z));
	}

	inline Matrix4x4f Translate(float x, float y, float z)
	{
		return glm::translate(Vec3f(x, y, z));
	}

	inline float DotProduct(Vec3f const& x, Vec3f const& y)
	{
		return glm::dot(x, y);
	}

	inline Matrix4x4f FromToRotation(Vec3f x, Vec3f y)
	{
		x = NormalizeVec(x);
		y = NormalizeVec(y);			
		auto axis = CrossProduct(x, y);
		float cosAngle = DotProduct(x, y);
		float angle = std::acosf(cosAngle);
		assert(!std::isnan(angle));
		/*
		if (std::isnan(angle))
		{
			if (cosAngle > 0)
				return Matrix4x4f(1.0f);
			else
				return -Matrix4x4f(1.0f);
		}*/
		return glm::rotate(angle, axis);		
	}


	inline Matrix4x4f FromToRotation(Vec3f x, Vec3f y, Vec3f upward)
	{
		x = NormalizeVec(x);
		y = NormalizeVec(y);
		if (VecLength(x - y) < 0.001f)
		{
			return Matrix4x4f(1.0f);
		}
		else if (VecLength(x + y) < 0.001f)
		{
			return Rotate(PI, upward);
		}
		else
		{
			auto axis = CrossProduct(x, y);
			float cosAngle = DotProduct(x, y);
			float angle = glm::acos(cosAngle);
			return glm::rotate(angle, axis);
		}


	}

	inline float RadsBetween(Vec3f x, Vec3f y)
	{
		x = NormalizeVec(x);
		y = NormalizeVec(y);
		float cosAngle = DotProduct(x, y);
		cosAngle = std::clamp(cosAngle, -1.0f, 1.0f);
		float angle = std::acosf(cosAngle);
		assert(!std::isnan(angle));
		return angle;
		/*
		if (VecLength(x - y) < 0.001f)
		{
			return 0.0f;
		}
		else if (VecLength(x + y) < 0.001f)
		{
			return PI;
		}
		else
		{
			
		}*/
	}
	
	inline Matrix4x4f Translate(Vec4f const& vec)
	{
		return Translate(Vec3f(vec));

	}

	inline std::string VecToString(Vec3f const& vec)
	{
		return glm::to_string(vec);
	}

	inline std::string VecToString(Vec4f const& vec)
	{
		return glm::to_string(vec);
	}

	inline std::string MatToString(Matrix4x4f const& mat)
	{
		return glm::to_string(mat);
	}

	inline void Decompose(Matrix4x4f const& transformation, Vec4f& translation, Quaternion4f& rotation, Vec4f& scale)
	{
		translation = transformation[3];
		float scaleX = VecLength(transformation[0]);
		float scaleY = VecLength(transformation[1]);
		float scaleZ = VecLength(transformation[2]);
		scale = { scaleX, scaleY, scaleZ, 0.0f };
		Matrix4x4f rotMatrix = transformation;
		rotMatrix[3] = { 0.0f, 0.0f, 0.0f, 1.0f };
		rotMatrix[0] /= scaleX;
		rotMatrix[1] /= scaleY;
		rotMatrix[2] /= scaleZ;
		rotation = RotToQuat(rotMatrix);
	}

	inline Matrix4x4f Compose(Vec3f const& T, Quaternion4f const& R, Vec3f const& S)
	{
		return glm::translate(T) * glm::toMat4(R) * glm::scale(S);
	}

	inline Matrix4x4f Compose(Vec4f const& T, Quaternion4f const& R, Vec4f const& S)
	{
		return glm::translate(Vec3f(T.x, T.y, T.z)) * glm::toMat4(R) * glm::scale(Vec3f(S.x, S.y, S.z));
	}


	float Adder(float a, float b);

	template<typename T>
	inline T LinearInterpolate(T const& t1, T const& t2, float alpha)
	{
		return t1 * alpha + t2 * (1.0f - alpha);
	}

	inline Quaternion4f SLerp(Quaternion4f const& q1, Quaternion4f const& q2, float alpha)
	{
		return glm::slerp(q1, q2, 1.0f - alpha);
	}

	inline float RandFloat(float min, float max)
	{
		return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
	}

	inline int RandInt(int min, int max)
	{
		return 0;
	}

	inline bool RandTest(float probability)
	{
		return (static_cast<float>(rand()) / static_cast<float>((RAND_MAX + 1.0f))) < probability;
	}

	inline float Distance2(Matrix4x4f const& mat1, Matrix4x4f const& mat2)
	{
		return glm::distance2(mat1[0], mat2[0]) + glm::distance2(mat1[1], mat2[1]) + glm::distance2(mat1[2], mat2[2]) + glm::distance2(mat1[3], mat2[3]);
	}

	inline float Distance(Matrix4x4f const& mat1, Matrix4x4f const& mat2)
	{
		return glm::sqrt(Distance2(mat1, mat2));
	}

	inline float Distance(Vec3f const& p1, Vec3f const& p2)
	{
		return glm::distance(p1, p2);
	}

	inline float Distance2(Vec3f const& p1, Vec3f const& p2)
	{
		return glm::distance2(p1, p2);
	}

}
