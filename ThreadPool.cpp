#include "ThreadPool.h"

void ThreadPool::quit() {
  std::lock_guard l(m);
  end = true;
  cv.notify_all();
}

void ThreadPool::start() {
  std::lock_guard l(m);
  end = false;
  coda.clear();
  cv.notify_all();
}

void ThreadPool::submit(std::function<void()> f) {
  std::lock_guard l(m);
  if (end == false) {
    std::packaged_task<void()> pk(f);
    coda.push_back(std::move(pk));
    cv.notify_one();
  }
}

void ThreadPool::runThread() {
  while (true) {
    std::unique_lock u(m);
    if (coda.empty() && end == false)
      cv.wait(u, [&]() { return !coda.empty() || end == true; });
    if (end == true && coda.empty()) {
      u.unlock();
      return;
    }
    auto f = std::move(coda.front());
    coda.pop_front();
    u.unlock();
    f();
  }
}
