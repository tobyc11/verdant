#include "TaskQueue.h"
#include "Task.h"
#include <algorithm>
#include <cstdio>
#include <memory>
#include <mutex>
#include <vector>

namespace verdant {
TaskQueue &TaskQueue::default_queue() {
  static TaskQueue q;
  return q;
}

const Task *TaskQueue::enqueue(std::function<void()> fn) {
  auto task = std::make_unique<Task>(std::move(fn));
  const Task *task_handle = task.get();

  std::unique_lock<std::mutex> lk(this_mut);
  tasks.push_back(std::move(task));
  cv_has_work.notify_one();

  if (debug_print) {
    fprintf(stderr, "+Task %p Signalling\n", task_handle);
  }

  return task_handle;
}

const Task *
TaskQueue::enqueue_await_all(std::function<void()> fn,
                             const std::vector<const Task *> prerequisites) {
  auto task = std::make_unique<Task>(std::move(fn));
  const Task *task_handle = task.get();

  std::unique_lock<std::mutex> lk(this_mut);
  for (const Task *prereq : prerequisites) {
    for (const std::unique_ptr<Task> &source : tasks) {
      if (source.get() == prereq) {
        source->add_successor(task.get());
      }
    }

    for (Task *source : tasks_running) {
      if (source == prereq) {
        source->add_successor(task.get());
      }
    }
  }

  if (task->is_ready()) {
    cv_has_work.notify_one();
  }

  if (debug_print) {
    if (task->is_ready()) {
      fprintf(stderr, "+Task %p Signalling\n", task_handle);
    } else {
      fprintf(stderr, "+Task %p with %u dependencies\n", task_handle,
              task_handle->get_prereq_count());
    }
  }

  tasks.push_back(std::move(task));
  return task_handle;
}

void TaskQueue::enqueue_shutdown() {
  std::unique_lock<std::mutex> lk(this_mut);
  shutdown = true;
  cv_has_work.notify_all();
}

std::unique_ptr<Task> TaskQueue::dequeue_task() {
  std::unique_lock<std::mutex> lk(this_mut);

  // Find the first ready task
  std::vector<std::unique_ptr<Task>>::iterator task_iter;
  while (true) {
    task_iter = std::find_if(tasks.begin(), tasks.end(),
                             [](const auto &task) { return task->is_ready(); });
    if (task_iter == tasks.end()) {
      if (shutdown) {
        // Drained all work, shutting down
        return {};
      }
      cv_has_work.wait(lk);
    } else {
      break;
    }
  }

  if (debug_print) {
    fprintf(stderr, "*Task %p\n", (*task_iter).get());
  }

  auto result = std::move(*task_iter);
  tasks.erase(task_iter);
  tasks_running.insert(result.get());
  return result;
}

void TaskQueue::retire_task(std::unique_ptr<Task> task) {
  std::unique_lock<std::mutex> lk(this_mut);
  tasks_running.erase(task.get());
  bool has_ready = task->decrement_successors();
  if (has_ready) {
    cv_has_work.notify_one();
  }

  if (debug_print) {
    if (!has_ready) {
      fprintf(stderr, "-Task %p\n", task.get());
    } else {
      fprintf(stderr, "-Task %p Signalling\n", task.get());
    }
  }
}
} // namespace verdant
