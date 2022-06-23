#pragma once
#include "MathDefs.h"
#include <memory>
#include <string>

namespace verdant {
class HDRImage {
public:
  HDRImage(const std::string &file_name);

  bool is_valid() const { return valid; }

  float3 get_color_spherical(float theta, float phi) {
    int y = theta / M_PI * height;
    int x = phi / M_PI / 2 * width;
    int i = y * width + x;
    return s[i];
  }

private:
  bool valid;
  int width;
  int height;
  // Image storage
  std::unique_ptr<float3[]> s;
};
} // namespace verdant
