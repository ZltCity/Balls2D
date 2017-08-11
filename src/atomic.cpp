#include "atomic.h"

AtomicLock::AtomicLock() {}

void AtomicLock::lock() {
  while (this->locked.test_and_set()) {}
}

void AtomicLock::unlock() {
  this->locked.clear();
}

bool AtomicLock::try_lock() {
  return !this->locked.test_and_set();
}
