#include "Utility.h"

#include "Catch.hpp"


bool vectorApprox(glm::vec2 v1, glm::vec2 v2, float margin) {
	return (v1.x == Approx(v2.x).margin(margin)) && (v1.y == Approx(v2.y).margin(margin));
}