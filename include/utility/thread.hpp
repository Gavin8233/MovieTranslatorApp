
#ifndef THREAD_H
#define THREAD_H

#include <condition_variable>
#include <mutex>
#include <thread>

struct Thread {

    std::thread thread;
    std::mutex mutex;
    std::condition_variable condition;

};



#endif