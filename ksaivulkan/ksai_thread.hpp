#pragma once
#include "ksai_config.hpp"

namespace ksai {
class Thread {
public:
    Thread()
    {
        mWorker = std::thread(&Thread::QueueLoop, this); // Thread(Function, This Parameter) THis parameter implicitly jaanu paryon ta tei vaera
    }

    ~Thread()
    {
        if (mWorker.joinable()) {
            Wait();
            mQueueMutex.lock();
            mDestroying = true;
            mCondition.notify_one();
            mQueueMutex.unlock();
            mWorker.join();
        }
    }

    void AddJob(std::function<void()> inFunction)
    {
        std::scoped_lock<std::mutex> lock(mQueueMutex);
        mJobQueue.push(mv(inFunction));
        mCondition.notify_one();
    }

    void Wait()
    {
        std::unique_lock<std::mutex> lock(mQueueMutex);
        mCondition.wait(lock, [this]() { return mJobQueue.empty(); });
    }

private:
    bool mDestroying = false;
    std::thread mWorker;
    std::queue<std::function<void()>> mJobQueue;
    std::mutex mQueueMutex;
    std::condition_variable mCondition;
    void QueueLoop()
    {
        while (true) {
            std::function<void()> job;

            {
                std::unique_lock<std::mutex> lock(mQueueMutex);
                mCondition.wait(lock, [this] { return (not mJobQueue.empty()) or mDestroying; });
                if (mDestroying) {
                    break;
                }
                job = mJobQueue.front();
            }
            job();
            {
                std::lock_guard<std::mutex> lock(mQueueMutex);
                mJobQueue.pop();
                mCondition.notify_one();
            }
        }
    }
};

struct ThreadPool {
    void SetThreadCount(ui inCount)
    {
        mThreads.clear();
        for (auto i = 0; i < inCount; i++) {
            mThreads.emplace_back(std::make_unique<Thread>());
        }
    }

    void Wait()
    {
        for (auto& t : mThreads) {
            t->Wait();
        }
    }
    v<up<Thread>> mThreads;
};
}