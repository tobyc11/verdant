#pragma once
#include "TaskQueue.h"
#include <thread>

namespace verdant {
class TaskWorker {
public:
  TaskWorker(TaskQueue &task_queue);

  void join() { thread.join(); }

  static void init_default_workers();
  static void shutdown_default_workers();

protected:
  void worker();

private:
  std::thread thread;
  TaskQueue &task_queue;
};
} // namespace verdant
