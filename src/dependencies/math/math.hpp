#pragma once
#include "dependencies/std_include.hpp"

namespace math
{
	float angle_normalize_360(const float angle);
	float angle_normalize_180(const float angle);
	float angle_delta(const float angle, const float angle2);

	inline Vec3 angle_normalize_360(const Vec3& v)
	{
		return
		{
			math::angle_normalize_360(v.x),
			math::angle_normalize_360(v.y),
			math::angle_normalize_360(v.z),
		};
	}

	inline Vec3 angle_normalize_180(const Vec3& v)
	{
		return
		{
			math::angle_normalize_180(v.x),
			math::angle_normalize_180(v.y),
			math::angle_normalize_180(v.z),
		};
	}

	inline Vec3 angle_delta(const Vec3& angles1, const Vec3& angles2)
	{
		return math::angle_normalize_180(angles1 - angles2);
	}
}
