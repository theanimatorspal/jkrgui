#pragma once
#include "Network.hpp"

namespace Jkr::Network {
template <typename T> class TsQueue {
    public:
    TsQueue()                  = default;
    TsQueue(const TsQueue<T>&) = delete;
    ~TsQueue() { clear(); }
    const T& front() {
        std::scoped_lock<std::mutex> lock(mQueueMutex);
        return mQueue.front();
    }

    const T& back() {
        std::scoped_lock<std::mutex> lock(mQueueMutex);
        return mQueue.back();
    }

    void push_back(const T& inItem) {
        std::scoped_lock<std::mutex> lock(mQueueMutex);
        return mQueue.push_back(std::move(inItem));
    }

    void push_front(const T& inItem) {
        std::scoped_lock<std::mutex> lock(mQueueMutex);
        return mQueue.push_front(std::move(inItem));
    }

    bool empty() {
        std::scoped_lock<std::mutex> lock(mQueueMutex);
        return mQueue.empty();
    }

    size_t count() {
        std::scoped_lock<std::mutex> lock(mQueueMutex);
        return mQueue.size();
    }

    void clear() {
        std::scoped_lock<std::mutex> lock(mQueueMutex);
        mQueue.clear();
    }

    T pop_front() {
        std::scoped_lock<std::mutex> lock(mQueueMutex);
        auto t = std::move(mQueue.front());
        mQueue.pop_front();
        return t;
    }

    T pop_back() {
        std::scoped_lock<std::mutex> lock(mQueueMutex);
        auto t = std::move(mQueue.back());
        mQueue.pop_back();
        return t;
    }

    protected:
    std::mutex mQueueMutex;
    std::deque<T> mQueue;
};
} // namespace Jkr::Network