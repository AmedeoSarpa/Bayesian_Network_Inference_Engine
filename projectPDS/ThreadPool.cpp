#include <memory>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <functional>
#include <future>

class ThreadPool{
private :
    std::mutex m;
    bool end = false;
    std::deque<  std::packaged_task<void()> > coda ;
    std::condition_variable cv;

public :
    void quit(){
        std::lock_guard l(m);
        end = true;
        cv.notify_all();
    }

    void submit(std::function<void()> f){
        std::lock_guard l(m);
        if (end == false ) {
            std::packaged_task<void()> pk(f);
            coda.push_back(std::move(pk));
            cv.notify_one();
        }
    }

    void runThread(){
        while(true) {
            std::unique_lock u(m);
            if (coda.empty() && end == false) cv.wait(u, [&]() { return !coda.empty() || end == true; });
            if (end == true && coda.empty()) {u.unlock();return;}
            auto f = std::move(coda.front());
            coda.pop_front();
            u.unlock();
            f();
        }
    }
};