#include "threadpool.h"

ThreadPool::ThreadPool(size_t threads) {
  this->alloc(threads);
}

ThreadPool::~ThreadPool() {
  this->free();
}

size_t ThreadPool::getThreadsCount() const {
  return this->threads.size();
}

void ThreadPool::alloc(size_t threads) {
  auto proxyFunction = [](ThreadPool::ThreadContext *context) {
    //context->entryMutex->lock();
    //context.routine();
    //context.entryMutex->unlock();
  };

  for (size_t i = 0; i < threads; ++i) {
    ThreadContext context = {
      i,
      SharedMutex(new std::mutex(), [](std::mutex *m) { m->unlock(); }),
      UserRoutine()
    };

    //context.entryMutex->lock();

    this->threads.push_back(std::make_pair(context, SharedThread(nullptr)));
    this->threads[i].second = SharedThread(new std::thread(proxyFunction, &this->threads[i].first));
  }
}

void ThreadPool::free() {
  for (std::pair<ThreadContext, SharedThread> &threadPair : this->threads)
    threadPair.second->detach();
}
