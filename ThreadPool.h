#ifndef PROJECTPDS__THREADPOOL_H_
#define PROJECTPDS__THREADPOOL_H_

#include <mutex>
#include <condition_variable>
#include <deque>
#include <functional>
#include <future>

class ThreadPool {
 private :
  std::mutex m;
  bool end = false;
  std::deque<std::packaged_task<void()>> coda;
  std::condition_variable cv;


 public :
  void quit();
  void start();
  void submit(std::function<void()> f);
  void runThread();
};

#endif //PROJECTPDS__THREADPOOL_H_
