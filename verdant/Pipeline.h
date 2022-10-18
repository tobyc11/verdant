#pragma once
#include "Camera.h"
#include "Film.h"
#include "Scene.h"
#include <atomic>
#include <memory>

namespace verdant {
enum class EventType { TileCompleted, NoLongerRunning };

typedef void (*EventCallback)(void *user_data, EventType event_type);

class PathTracePipeline {
public:
  PathTracePipeline(unsigned int width, unsigned int height,
                    unsigned int samples);

  std::shared_ptr<Scene> get_scene() const { return scene; }
  std::shared_ptr<Camera> get_camera() const { return camera; }
  std::shared_ptr<Film> get_film() const { return film; }

  // Both run and stop are async. Use event_callback to listen to events.
  void run(const std::string &file_name, bool write = true);
  void stop();

  void single_pixel(unsigned int x, unsigned int y);

  void set_event_callback(EventCallback fn, void *data) {
    event_callback = fn;
    user_data = data;
  }

  void get_status(bool &is_running, int &tiles_total,
                  int &tiles_completed) const;

protected:
  void render_tile(unsigned int x, unsigned int y, unsigned int x_len,
                   unsigned int y_len);

private:
  unsigned int samples;
  std::shared_ptr<Scene> scene;
  std::shared_ptr<Camera> camera;
  std::shared_ptr<Film> film;

  bool is_running = false;
  bool stop_flag;
  int tiles_total;
  std::atomic_int tiles_completed;

  EventCallback event_callback = nullptr;
  void *user_data;
};
} // namespace verdant
