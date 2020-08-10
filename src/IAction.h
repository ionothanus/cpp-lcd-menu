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