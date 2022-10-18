#include "Pipeline.h"
#include "PathTracer.h"
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

void PathTracePipeline::run(bool write) {
  const unsigned int tile_len = 256;

  // Cannot call run when it is already running
  if (is_running) {
    return;
  }

  is_running = true;
  stop_flag = false;
  tiles_total = 0;
  tiles_completed = 0;

  unsigned int y, x;
  std::vector<const Task *> tasks;
  for (y = 0; y < film->get_height(); y += tile_len) {
    for (x = 0; x < film->get_width(); x += tile_len) {
      tiles_total += 1;
      tasks.push_back(TaskQueue::default_queue().enqueue(
          [this, x, y, tile_len]() { render_tile(x, y, tile_len, tile_len); }));
    }
  }

  TaskQueue::default_queue().enqueue_await_all(
      [this]() {
        is_running = false;
        if (event_callback)
          event_callback(user_data, EventType::NoLongerRunning);
      },
      tasks);

  if (write) {
    TaskQueue::default_queue().enqueue_await_all(
        [this]() { film->write_to_ppm("hello.ppm"); }, tasks);
  }
}

void PathTracePipeline::stop() { stop_flag = true; }

void PathTracePipeline::single_pixel(unsigned int x, unsigned int y) {
  PathTracer integrator(*scene, UniformSampler::per_thread());

  Ray ray = camera->generate_ray_from_uv(film->xy_to_uv(x, y));
  ray.origin.z() += 5.0f;
  float3 Li = integrator.radiance(ray);
  film->average_radiance(x, y, Li);
}

void PathTracePipeline::get_status(bool &is_running, int &tiles_total,
                                   int &tiles_completed) const {
  is_running = this->is_running;
  tiles_total = this->tiles_total;
  tiles_completed = this->tiles_completed;
}

void PathTracePipeline::render_tile(unsigned int x_begin, unsigned int y_begin,
                                    unsigned int x_len, unsigned int y_len) {
  PathTracer integrator(*scene, UniformSampler::per_thread());
  unsigned int x, y;
  for (y = y_begin; y < y_begin + y_len && y < film->get_height(); y++) {
    for (x = x_begin; x < x_begin + x_len && x < film->get_width(); x++) {
      unsigned int t;
      Ray ray = camera->generate_ray_from_uv(film->xy_to_uv(x, y));
      ray.origin.z() += 5.0f;
      for (t = 0; t < samples; t++) {
        float3 Li = integrator.radiance(ray);
        film->average_radiance(x, y, Li);

        if (stop_flag) {
          return;
        }
      }
    }
  }

  tiles_completed.fetch_add(1);
  if (event_callback)
    event_callback(user_data, EventType::TileCompleted);
}
} // namespace verdant