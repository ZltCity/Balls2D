#pragma once

#include <atomic>

class AtomicLock {
public:
  AtomicLock();

  void lock();
  void unlock();

  bool try_lock();

private:
  AtomicLock(const AtomicLock &) = delete;
  AtomicLock &operator=(const AtomicLock &) = delete;

  //  ќказываетс€ так можно, но видимо только в последних стандартах.
  //  ToDo: ¬ыпилить данный коммент после проверки работоспособности на GCC.
  std::atomic_flag locked = ATOMIC_FLAG_INIT;
};
