// Copyright (c) 2020 Jonathan Moscardini
// 
// Licensed under the MIT License
// http://www.opensource.org/licenses/MIT

#ifndef SRC_ITASK_H
#define SRC_ITASK_H

namespace textmenu
{
    class ITask
    {
    public:
        virtual void Run() = 0;
        virtual void Stop() = 0;
        virtual ~ITask() = default;
    };
}

#endif //SRC_ITASK_H