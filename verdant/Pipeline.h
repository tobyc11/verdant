#pragma once
#include "Camera.h"
#include "Film.h"
#include "Integrator.h"
#include "Scene.h"
#include <memory>

namespace verdant {
class PathTracePipeline {
public:
  PathTracePipeline(unsigned int width, unsigned int height,
                    unsigned int samples);

  std::shared_ptr<Scene> get_scene() const { return scene; }
  std::shared_ptr<Camera> get_camera() const { return camera; }
  std::shared_ptr<Film> get_film() const { return film; }

  void run();
  void single_pixel(unsigned int x, unsigned int y);

protected:
  void render_tile(unsigned int x, unsigned int y, unsigned int x_len,
                   unsigned int y_len);

private:
  unsigned int samples;
  std::shared_ptr<Scene> scene;
  std::shared_ptr<Camera> camera;
  std::shared_ptr<Film> film;
};
} // namespace verdant
