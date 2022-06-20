#pragma once
#include "MathDefs.h"

namespace minirt {
class Camera {
public:
  // Field of view in the Y axis in radians
  float get_fov_y() const { return fov_y; }
  void set_fov_y(float value) { fov_y = value; }

  // Width divided by height
  float get_aspect_ratio() const { return aspect_ratio; }
  void set_aspect_ratio(float value) { aspect_ratio = value; }

  // Not used for ray tracing
  float get_near_z() const { return near_z; }
  void set_near_z(float value) { near_z = value; }

  // Not used for ray tracing
  float get_far_z() const { return far_z; }
  void set_far_z(float value) { far_z = value; }

  // By convention, x is always to the right and y is always top
  // z is a bit more unclear
  bool get_forward_is_positive_z() const { return forward_is_positive_z; }
  void set_forward_is_positive_z(bool value) { forward_is_positive_z = value; }

  // NDC is in the range [-1, 1]
  // (-1, -1) is the bottom left corner
  Ray generate_ray_from_ndc(float2 ndc);

  // UV is in the range of [0, 1]
  // (0, 0) is the top left corner of the camera view
  Ray generate_ray_from_uv(float2 uv);

  float2 uv_to_ndc(float2 uv);

private:
  // Property backings
  float fov_y;
  float aspect_ratio;
  float near_z = 1.0f;
  float far_z = 1000.0f;
  bool forward_is_positive_z = false;
};
} // namespace minirt
