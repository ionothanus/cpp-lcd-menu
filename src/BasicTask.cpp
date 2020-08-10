
#include "BasicTask.h"

namespace textmenu
{
    BasicTask::BasicTask()
        : m_thread{},
          m_cv_mutex{},
          m_thread_cv{},
          m_terminate{false}
    {}

    void BasicTask::Run()
    {
        if (!m_thread.joinable())
        {
            m_terminate = false;
            m_thread = std::thread{[this](){ ThreadFunction(); } };
        }
    }

    void BasicTask::Stop()
    {
        m_terminate = true;
        m_thread_cv.notify_all();

        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }
}