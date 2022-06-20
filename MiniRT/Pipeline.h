#pragma once
#include "Camera.h"
#include "Film.h"
#include "Integrator.h"
#include "Scene.h"
#include <memory>

namespace minirt {
class PathTracePipeline {
public:
  PathTracePipeline(unsigned int width, unsigned int height,
                    unsigned int samples);

  std::shared_ptr<Scene> get_scene() const { return scene; }
  std::shared_ptr<PathTraceIntegrator> get_integrator() const {
    return integrator;
  }
  std::shared_ptr<Camera> get_camera() const { return camera; }
  std::shared_ptr<Film> get_film() const { return film; }

  void run();

private:
  unsigned int samples;
  std::shared_ptr<Scene> scene;
  std::shared_ptr<PathTraceIntegrator> integrator;
  std::shared_ptr<Camera> camera;
  std::shared_ptr<Film> film;
};
} // namespace minirt
