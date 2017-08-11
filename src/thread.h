#pragma once

#include <thread>
#include <memory>

class Worker {
public:
  Worker() = default;
  virtual ~Worker() = 0;

  virtual void exec() = 0;

private:
  Worker(const Worker &) = delete;
  Worker &operator=(const Worker &) = delete;
};

typedef std::shared_ptr<Worker> WorkerPtr;

class Thread {
public:
  Thread() = default;
  Thread(Thread &&thread);
  Thread(WorkerPtr &worker);

  Thread &operator=(Thread &&thread);

  void exec(WorkerPtr &worker);
  void wait();

  std::thread &getStlThread();
  const std::thread &getStlThread() const;

private:
  Thread(const Thread &) = delete;
  Thread &operator=(const Thread &) = delete;

  std::thread stlThread;
};
