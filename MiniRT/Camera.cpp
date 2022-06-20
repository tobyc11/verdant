#include "Camera.h"

namespace minirt {
Ray Camera::generate_ray_from_ndc(float2 ndc) {
  float z = -1.0f;
  if (forward_is_positive_z) {
    z = 1.0f;
  }
  float y_top_right = tan(get_fov_y() / 2.0f);
  float x_top_right = y_top_right * get_aspect_ratio();
  float3 viewRayDir = float3(x_top_right * ndc.x(), y_top_right * ndc.y(), z);
  float3 origin = float3(0, 0, 0);

  Ray ray(origin, normalize(viewRayDir));
  return ray;
}

Ray Camera::generate_ray_from_uv(float2 uv) {
  return generate_ray_from_ndc(uv_to_ndc(uv));
}

float2 Camera::uv_to_ndc(float2 uv) {
  float2 temp = uv * 2 - 1;
  return {temp.x(), -temp.y()};
}
} // namespace minirt
