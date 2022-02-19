#include "dependencies/std_include.hpp"
#include "math.hpp"

namespace math
{
	float angle_delta(const float angle, const float angle2) 
	{
		return game::AngleNormalize180(angle - angle2);
	}
}