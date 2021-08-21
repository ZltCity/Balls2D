#include "threadpool.h"

class ProxyWorker : public Worker {
public:
  ProxyWorker(TaskList &taskList, bool &quitFlag)
    : taskList(taskList), quitFlag(quitFlag) {}

  void exec() {
    while (!this->quitFlag)
      doTask(this->taskList);
  }

private:
  ProxyWorker(const ProxyWorker &) = delete;
  ProxyWorker &operator=(const ProxyWorker &) = delete;

  TaskList &taskList;
  bool &quitFlag;
};

Task::~Task() {}

bool Task::done() const {
  return this->doneFlag;
}

void Task::setDone() {
  this->doneFlag = true;
}

void Task::resetDone() {
  this->doneFlag = false;
}

void TaskList::push(TaskPtr &taskPtr) {
  std::unique_lock<std::mutex> lock(this->entryLock);

  this->list.push_back(taskPtr);
  this->waitCond.notify_one();
}

void TaskList::resumeAll() {
  this->waitCond.notify_all();
}

void doTask(TaskList &taskList) {
  std::unique_lock<std::mutex> lock(taskList.entryLock);
  std::list<TaskPtr> &list = taskList.list;
  TaskPtr taskPtr(nullptr);

  if (list.size() == 0) {
    taskList.wait(lock);

    return;
  }

  taskPtr = list.front();
  list.pop_front();
  lock.unlock();
  taskPtr->doTask();
  taskPtr->setDone();
}

void TaskList::wait(std::unique_lock<std::mutex> &lock) {
  this->waitCond.wait(lock);
}

ThreadPool::ThreadPool()
  : quitFlag(false) {}

ThreadPool::~ThreadPool() {
  this->free();
}

void ThreadPool::alloc(size_t count) {
  this->pool.resize(count);
  //
  for (size_t i = 0; i < count; ++i) {
    Thread thread(WorkerPtr(new ProxyWorker(this->taskList, this->quitFlag)));

    this->pool[i] = std::move(thread);
  }
}

void ThreadPool::pushTask(TaskPtr &taskPtr) {
  this->taskList.push(taskPtr);
}

size_t ThreadPool::getCount() const {
  return this->pool.size();
}

void ThreadPool::free() {
  this->quitFlag = true;
  //
  this->taskList.resumeAll();

  for (size_t i = 0; i < this->getCount(); ++i)
    this->pool[i].wait();
}
