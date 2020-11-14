// Copyright (c) 2020 Jonathan Moscardini
// 
// Licensed under the MIT License
// http://www.opensource.org/licenses/MIT

#ifndef SRC_IACTION_H
#define SRC_IACTION_H

namespace textmenu
{
    class IAction
    {
    public:
        virtual void Execute() = 0;
    };
}

#endif //SRC_IACTION_H