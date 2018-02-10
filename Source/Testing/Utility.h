#pragma once
#include <glm/glm.hpp>

bool vectorApprox(glm::vec2 v1, glm::vec2 v2, float margin = 0);

static const float k_margin = 0.00001f;