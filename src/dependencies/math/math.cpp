#include "dependencies/std_include.hpp"
#include "math.hpp"

namespace math
{
	float angle_normalize_360(const float angle)
	{
		if (angle >= 360.0f || angle < 0.0f)
		{
			return angle - std::floorf(angle / 360.0f) * 360.0f;
		}

		return angle;
	}

	float angle_normalize_180(const float angle)
	{
		const auto new_angle = math::angle_normalize_360(angle);

		if (new_angle > 180.0f)
			return new_angle - 360.0f;

		return new_angle;
	}

	float angle_delta(const float angle, const float angle2)
	{
		return math::angle_normalize_180(angle - angle2);
	}
}