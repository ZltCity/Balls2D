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

  //  ����������� ��� �����, �� ������ ������ � ��������� ����������.
  //  ToDo: �������� ������ ������� ����� �������� ����������������� �� GCC.
  std::atomic_flag locked = ATOMIC_FLAG_INIT;
};
