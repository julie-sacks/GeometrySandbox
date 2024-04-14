#pragma once

#include <utility>
#include <glm/vec3.hpp>
#include <vector>

std::pair<std::vector<glm::vec3>, std::vector<glm::uvec3>> GenSphere(int steps = 16);

std::pair<std::vector<glm::vec3>, std::vector<glm::uvec3>> GenCylinder(int steps = 16);
