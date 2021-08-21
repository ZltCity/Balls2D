#include "thread.h"

Worker::~Worker() {}

Thread::Thread(Thread &&thread) {
  this->stlThread = std::move(thread.getStlThread());
}

Thread::Thread(WorkerPtr &worker) {
  this->exec(worker);
}

Thread &Thread::operator=(Thread &&thread) {
  this->stlThread = std::move(thread.getStlThread());

  return *this;
}

void Thread::exec(WorkerPtr &worker) {
  this->stlThread = std::thread(
    [worker]() {
      worker->exec();
    }
  );
}

void Thread::wait() {
  this->stlThread.join();
}

std::thread &Thread::getStlThread() {
  return this->stlThread;
}

const std::thread &Thread::getStlThread() const {
  return this->stlThread;
}
