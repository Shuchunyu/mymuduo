#include "Thread.h"
#include "CurrentThread.h"

#include<semaphore.h>

std::atomic_int Thread::numCreated_ {0};

Thread::Thread(ThreadFunc func, const std::string &name)
    :started_(false)
    ,joined_(false)
    ,tid_(0)
    ,func_(std::move(func))
    ,name_(name)
{
    setDefaultName();
}

Thread::~Thread()
{
    if(started_ && !joined_)
    {
        thread_ -> detach();
    }
}

// 真正启动线程
void Thread::start()
{
    sem_t sem;
    sem_init(&sem, false, 0);
    started_ = true;
    thread_ = std::shared_ptr<std::thread>(new std::thread([&](){
        tid_ = CurrentThread::tid();
        sem_post(&sem);
        func_();                //子线程执行此函数
    }));
    //等待获取上面新创建的线程的tid
    sem_wait(&sem);
}

void Thread::join()
{
    joined_ = true;
    thread_ -> join();
}


void Thread::setDefaultName()
{
    int num = ++numCreated_;
    if(name_.empty())
    {
        char buf[32] = {0};
        snprintf(buf, sizeof buf, "Thread %d", num);
        name_ = buf;
    }
}