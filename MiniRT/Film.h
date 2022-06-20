#pragma once
#include "MathDefs.h"
#include <memory>

namespace minirt {
// AKA framebuffer
class Film {
public:
  Film(unsigned int width, unsigned int height);

  unsigned int get_width() const { return width; }
  unsigned int get_height() const { return height; }
  float get_aspect_ratio() const { return (float)width / height; }

  void clear();

  float3 get_radiance(unsigned int x, unsigned int y) const;
  void average_radiance(unsigned int x, unsigned int y, float3 Li);
  void accumulate_radiance(unsigned int x, unsigned int y, float3 Li);

  float2 xy_to_uv(unsigned int x, unsigned int y) const;

  /**
   * @brief Gets a raw array of R32G32B32 format, with row pitch 12 * width
   *
   * @return const void* Pointer to the data
   */
  const void *data() const { return s.get(); }

  /**
   * @brief Write the film data into a new RGB ppm file
   *
   * @param file_name Full name or path to the file
   */
  void write_to_ppm(const std::string &file_name) const;

  /**
   * @brief Clamp floats to the range [0, 1] and converts to 0-255
   *
   * @param radiance Input floats
   * @return uint3 Output integers between 0 and 255
   */
  static uint3 clamp_to_255(float3 radiance) {
    uint3 r;
    for (int i = 0; i < 3; i++) {
      float f = radiance[i];
      if (f < 0.0f) {
        f = 0.0f;
      } else if (f > 1.0f) {
        f = 1.0f;
      }
      r[i] = f * 255;
    }
    return r;
  }

  /**
   * @brief Clamp floats to the range [0, 1] and converts to 0-255
   *
   * @param radiance Input floats
   * @param max Input floats will be divided by this input
   * @param gamma Gamma correction exponent
   * @return uint3 Output integers between 0 and 255
   */
  static uint3 clamp_to_255_gamma(float3 radiance, float3 max, float gamma) {
    uint3 r;
    for (int i = 0; i < 3; i++) {
      float f = radiance[i];

      // Gamma mapping
      f = pow(f / max[i], gamma);

      if (f < 0.0f) {
        f = 0.0f;
      } else if (f > 1.0f) {
        f = 1.0f;
      }
      r[i] = f * 255;
    }
    return r;
  }

private:
  // Backing storage: r32 g32 b32
  std::unique_ptr<float3[]> s;
  std::unique_ptr<unsigned int[]> n;
  unsigned int width;
  unsigned int height;
};
} // namespace minirt
