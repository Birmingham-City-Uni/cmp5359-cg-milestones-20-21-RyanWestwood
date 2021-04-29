#pragma once
#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

inline double Degrees_To_Radians(double degrees) {
	return degrees * pi / 180;
}

#include "ray.h"
#include "geometry.h"