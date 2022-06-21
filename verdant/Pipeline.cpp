#include "Pipeline.h"

namespace verdant {
PathTracePipeline::PathTracePipeline(unsigned int width, unsigned int height,
                                     unsigned int samples)
    : samples(samples) {
  scene = std::make_shared<Scene>();
  integrator = std::make_shared<PathTraceIntegrator>();
  camera = std::make_shared<Camera>();
  film = std::make_shared<Film>(width, height);

  camera->set_aspect_ratio(film->get_aspect_ratio());
  camera->set_fov_y(M_PI / 180.f * 90.f);

  scene->add_point_light({2.5f, 0.0f, 2.5f}, {1.0f, 3.0f, 5.0f});
}

void PathTracePipeline::run() {
  auto total_film =
      std::make_shared<Film>(film->get_width(), film->get_height());

  unsigned int d;
  unsigned int x, y, t;
  for (d = 1; d <= 4; d++) {
    integrator->set_only_depth(d);
    film->clear();

    for (y = 0; y < film->get_height(); y++) {
      for (x = 0; x < film->get_width(); x++) {
        Ray ray = camera->generate_ray_from_uv(film->xy_to_uv(x, y));
        ray.origin.z() += 5.0f;
        for (t = 0; t < samples; t++) {
          float3 Li = integrator->Lo_from_ray(*scene, ray);
          film->average_radiance(x, y, Li);
        }
        total_film->accumulate_radiance(x, y, film->get_radiance(x, y));
      }
    }

    char file_name[256];
    sprintf(file_name, "hello%d.ppm", d);
    film->write_to_ppm(file_name);
  }
  total_film->write_to_ppm("hello.ppm");
}
} // namespace verdant