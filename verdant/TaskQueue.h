#pragma once
#include "Task.h"
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <set>
#include <vector>

namespace verdant {
/**
 * @brief A thread safe data structure to keep track of all pending tasks and
 * their dependencies
 *
 */
class TaskQueue {
public:
  static TaskQueue &default_queue();

  const Task *enqueue(std::function<void()> fn);

  const Task *enqueue_await_all(std::function<void()> fn,
                                const std::vector<const Task *> prerequisites);

  void enqueue_shutdown();

  // May sleep thread
  std::unique_ptr<Task> dequeue_task();

  void retire_task(std::unique_ptr<Task> task);

private:
  std::mutex this_mut;
  std::condition_variable cv_has_work;
  std::vector<std::unique_ptr<Task>> tasks;
  std::set<Task *> tasks_running;
  bool shutdown = false;
  bool debug_print = true;
};
} // namespace verdant
