#include "Pipeline.h"
#include "Integrator.h"
#include "PathIntegrator.h"
#include "Sampler.h"
#include "TaskQueue.h"
#include <memory>
#include <stdio.h>
#include <thread>
#include <vector>

namespace verdant {
PathTracePipeline::PathTracePipeline(unsigned int width, unsigned int height,
                                     unsigned int samples)
    : samples(samples) {
  scene = std::make_shared<Scene>();
  camera = std::make_shared<Camera>();
  film = std::make_shared<Film>(width, height);

  camera->set_aspect_ratio(film->get_aspect_ratio());
  camera->set_fov_y(M_PI / 180.f * 90.f);
}

void PathTracePipeline::run() {
  const unsigned int tile_len = 256;

  unsigned int y, x;
  std::vector<const Task *> tasks;
  for (y = 0; y < film->get_height(); y += tile_len) {
    for (x = 0; x < film->get_width(); x += tile_len) {
      tasks.push_back(TaskQueue::default_queue().enqueue(
          [this, x, y, tile_len]() { render_tile(x, y, tile_len, tile_len); }));
    }
  }
  TaskQueue::default_queue().enqueue_await_all(
      [this]() { film->write_to_ppm("hello.ppm"); }, tasks);
}

void PathTracePipeline::single_pixel(unsigned int x, unsigned int y) {
  UniformSampler sampler;
  PathIntegrator integrator(*scene, sampler);

  Ray ray = camera->generate_ray_from_uv(film->xy_to_uv(x, y));
  ray.origin.z() += 5.0f;
  float3 Li = integrator.Lo_from_ray(ray);
  film->average_radiance(x, y, Li);
}

void PathTracePipeline::render_tile(unsigned int x_begin, unsigned int y_begin,
                                    unsigned int x_len, unsigned int y_len) {
  UniformSampler sampler;
  PathIntegrator integrator(*scene, sampler);
  unsigned int x, y;
  for (y = y_begin; y < y_begin + y_len && y < film->get_height(); y++) {
    for (x = x_begin; x < x_begin + x_len && x < film->get_width(); x++) {
      unsigned int t;
      Ray ray = camera->generate_ray_from_uv(film->xy_to_uv(x, y));
      ray.origin.z() += 5.0f;
      for (t = 0; t < samples; t++) {
        float3 Li = integrator.Lo_from_ray(ray);
        film->average_radiance(x, y, Li);
      }
    }
  }
}
} // namespace verdant