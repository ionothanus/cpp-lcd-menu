#ifndef SRC_BASICTASK_H
#define SRC_BASICTASK_H

#include "ITask.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace textmenu
{
    class BasicTask : ITask
    {
    public:
        BasicTask();
        ~BasicTask() override = default;
        void Run() final override;
        void Stop() final override;

    protected:
        std::thread m_thread;
        std::mutex m_cv_mutex;
        std::condition_variable m_thread_cv;
        std::atomic<bool> m_terminate;

        virtual void ThreadFunction() = 0;
    };
}

#endif //SRC_BASICTASK_H