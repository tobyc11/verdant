#include "TaskWorker.h"
#include "TaskQueue.h"
#include <memory>
#include <thread>
#include <vector>

namespace {
const int worker_count = 4;
std::vector<std::unique_ptr<verdant::TaskWorker>> default_workers;
} // namespace

namespace verdant {
TaskWorker::TaskWorker(TaskQueue &task_queue) : task_queue(task_queue) {
  thread = std::thread(&TaskWorker::worker, this);
}

void TaskWorker::init_default_workers() {
  for (int i = 0; i < worker_count; i++) {
    default_workers.emplace_back(
        std::make_unique<TaskWorker>(TaskQueue::default_queue()));
  }
}

void TaskWorker::shutdown_default_workers() {
  TaskQueue::default_queue().enqueue_shutdown();
  for (int i = 0; i < worker_count; i++) {
    default_workers[i]->join();
  }
  default_workers.clear();
}

void TaskWorker::worker() {
  while (1) {
    std::unique_ptr<Task> task = task_queue.dequeue_task();
    if (!task) {
      break;
    }
    task->execute();
    task_queue.retire_task(std::move(task));
  }
}
} // namespace verdant
