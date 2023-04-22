#ifndef CS171_HW3_INCLUDE_UTILS_H_
#define CS171_HW3_INCLUDE_UTILS_H_

#include "core.h"
#include <iostream>

namespace utils {

static inline float clamp01(float v) {
  if (v > 1) v = 1;
  else if (v < 0) v = 0;
  return v;
}

static inline uint8_t gammaCorrection(float radiance) {
  return static_cast<uint8_t>(ceil(255 * pow(clamp01(radiance), 1 / 2.2)));
}

static inline float vec3fdiv(Vec3f v1, Vec3f dir) {
  // std::cout << "dir.pos: (" << dir.x() << ',' << dir.y() << ',' << dir.z() << ")\n";
  // std::cout << "v1.pos: (" << v1.x() << ',' << v1.y() << ',' << v1.z() << ")\n";
  if (dir.x() < -0.01 || dir.x() > 0.01)
    return v1.x() / dir.x();
  if (dir.y() < -0.01 || dir.y() > 0.01)
    return v1.y() / dir.y();
  if (dir.z() < -0.01 || dir.z() > 0.01)
    return v1.z() / dir.z();
  throw "Zero direction!\n";
}
}

#endif //CS171_HW3_INCLUDE_UTILS_H_
