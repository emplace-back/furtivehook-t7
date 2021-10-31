#include "dependencies/std_include.hpp"
#include "math.hpp"

namespace math
{
	float angle_normalize_180(const float angle) 
	{
		const auto scaledAngle = angle * 0.0027777778f;
		return (scaledAngle - std::floorf(scaledAngle + std::numeric_limits<float>::round_error())) * 360.0f;
	}
	
	float angle_delta(const float angle, const float angle2) {
		return angle_normalize_180(angle - angle2);
	}
}