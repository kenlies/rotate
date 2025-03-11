#pragma once

#include "constants.h"
#include <box2d/box2d.h>

namespace Utils {
	// calculate force direction in world space to acoount for rotated world // inline??
	inline b2Vec2 createForce(float force, float radians) {
		b2Vec2 forceVec;
		forceVec.x = -force * sin(radians);
		forceVec.y = force * cos(radians);
		return forceVec;
	}
}
